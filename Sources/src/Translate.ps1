param(
    [Parameter(Mandatory = $true)]
    [string]$SourcePath,

    [string]$TargetLang = "en"
)

# Make console output UTF-8 (so you see Russian / English correctly in the host)
$OutputEncoding = [Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)

# UTF-8 without BOM for file read/write
$Utf8NoBom = New-Object System.Text.UTF8Encoding($false)

# Encodings used to "fix" mojibake
$Latin1 = [System.Text.Encoding]::GetEncoding(28591)   # ISO-8859-1
$Cp1251 = [System.Text.Encoding]::GetEncoding(1251)    # Windows-1251 (Russian)

# Try to repair classic CP1251→Latin1 mojibake like "ñ÷èòàåò" -> "считает"
function Fix-RussianMojibake {
    param([string]$Text)

    if ([string]::IsNullOrWhiteSpace($Text)) { return $Text }

    # Heuristic: only bother if there are characters typical for mojibake
    if ($Text -notmatch '[ðñòíëèþÿöæ]') {
        return $Text
    }

    $bytes = $Latin1.GetBytes($Text)
    $fixed = $Cp1251.GetString($bytes)
    return $fixed
}

function Translate-Text {
    param(
        [string]$Text,
        [string]$TargetLang = "en"
    )

    if ([string]::IsNullOrWhiteSpace($Text)) {
        return $Text
    }

    # First attempt to repair mojibake back to real Russian
    $Text = Fix-RussianMojibake $Text

    # URL-encode for GET
    $encoded = [uri]::EscapeDataString($Text)

    # Force Russian -> English (or other target), no auto-detect
    $url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=ru&tl=$TargetLang&dt=t&q=$encoded"

    try {
        $raw  = Invoke-WebRequest -Uri $url -Method Get -UseBasicParsing
        $json = $raw.Content | ConvertFrom-Json

        # JSON structure: [ [ [ "translated", "original", ... ] ], ... ]
        $translated = $json[0][0][0]
        if ([string]::IsNullOrWhiteSpace($translated)) {
            return $Text
        }
        return $translated
    } catch {
        Write-Warning "Translation failed: $($_.Exception.Message)"
        return $Text
    }
}

function Get-FileTextUtf8 {
    param([string]$FilePath)

    $bytes  = [System.IO.File]::ReadAllBytes($FilePath)
    $string = $Utf8NoBom.GetString($bytes)
    return $string
}

function Set-FileTextUtf8 {
    param(
        [string]$FilePath,
        [string]$Text
    )

    # One-time backup next to the file
    if (-not (Test-Path "$FilePath.bak")) {
        Copy-Item $FilePath "$FilePath.bak" -ErrorAction SilentlyContinue
    }

    $bytes = $Utf8NoBom.GetBytes($Text)
    [System.IO.File]::WriteAllBytes($FilePath, $bytes)
}

function Translate-CommentsInFile {
    param([string]$FilePath)

    Write-Host "Processing file: $FilePath"

    $content = Get-FileTextUtf8 -FilePath $FilePath

    # Match //... (single line) and /* ... */ (block, possibly multiline)
    $pattern = '(?s)//.*?$|/\*.*?\*/'

    $translated = [regex]::Replace($content, $pattern, {
        param($match)

        $comment = $match.Value

        # Extract inner text: strip comment markers
        $inner = $comment `
            -replace '^\s*//', '' `
            -replace '^/\*', '' `
            -replace '\*/$', ''

        $inner = $inner.Trim()

        if ([string]::IsNullOrWhiteSpace($inner)) {
            return $comment
        }

        Write-Host "  Original comment: $inner"

        $translatedText = Translate-Text -Text $inner -TargetLang $TargetLang

        Write-Host "  Translated:       $translatedText"
        Write-Host ""

        # Rebuild the comment keeping style
        if ($comment.TrimStart().StartsWith("//")) {
            # Preserve leading whitespace before //
            $idx = $comment.IndexOf("//")
            $leading = if ($idx -gt 0) { $comment.Substring(0, $idx) } else { "" }
            return "$leading// $translatedText"
        } else {
            # Simple one-line block comment
            return "/* $translatedText */"
        }
    }, 'Multiline')

    Set-FileTextUtf8 -FilePath $FilePath -Text $translated
}

# Walk the directory and process source files
Get-ChildItem -Path $SourcePath -Recurse -Include *.cpp, *.h, *.cs | ForEach-Object {
    Translate-CommentsInFile -FilePath $_.FullName
}
