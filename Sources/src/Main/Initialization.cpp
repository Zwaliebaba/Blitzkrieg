#include "pch.h"

#include "iMain.h"

#include "../StreamIO/OptionSystem.h"
#include "../GFX/GFX.h"
#include "../GFX/GFXHelper.h"
#include "../GFX/GFXObjectFactory.h"
#include "../SFX/SFX.h"
#include "../SFX/SoundObjectFactory.h"
#include "../Input/Input.h"
#include "../Input/InputObjectFactory.h"
#include "../Anim/Animation.h"
#include "../Anim/AnimObjectFactory.h"
#include "../Scene/Scene.h"
#include "../Scene/Terrain.h"
#include "../Scene/PFX.h"
#include "../Scene/SceneObjectFactory.h"
#include "../AILogic/AILogic.h"
#include "../AILogic/DifficultyLevel.h"
#include "../AILogic/AILogicObjectFactory.h"
#include "../UI/UI.h"
#include "../UI/UIObjectFactory.h"
#include "../Image/Image.h"
#include "../Image/ImageObjectFactory.h"
#include "../Formats/fmtTerrain.h"
#include "../Main/TextSystem.h"
#include "../UI/MaskSystem.h"
#include "../Main/Transceiver.h"
#include "../GameTT/AckManager.h"
#include "../Main/ScenarioTracker.h"
#include "../GameTT/MultiplayerCommandManager.h"
#include "../GameTT/MissionObjectFactory.h"
#include "../Common/PauseGame.h"
#include "../Main/CommandsHistoryInterface.h"
#include "../GameTT/MessageReaction.h"
#include "../StreamIO/StreamIOObjectFactory.h"
#include "../Net/NetObjectFactory.h"
#include "../StreamIO/RandomGen.h"

namespace NMain
{
  bool bInitialized = false;
};

inline float MakeGammaValue(const variant_t &var) { return (static_cast<float>(var) - 50.0f) / 50.0f; }

bool STDCALL NMain::SwitchGame(bool bOn)
{
  if (bOn)
  {
    if (IsInitialized())
    {
      if (IMainLoop *pML = GetSingleton<IMainLoop>()) pML->Pause(false, PAUSE_TYPE_INACTIVE);
      // set game's gamma correction
      variant_t vtBrightness = 0.0f, vtContrast = 0.0f, vtGamma = 0.0f;
      GetSingleton<IOptionSystem>()->Get("GFX.Gamma.Brightness", &vtBrightness);
      GetSingleton<IOptionSystem>()->Get("GFX.Gamma.Contrast", &vtContrast);
      GetSingleton<IOptionSystem>()->Get("GFX.Gamma.Gamma", &vtGamma);
      SetGammaCorrection(MakeGammaValue(vtBrightness), MakeGammaValue(vtContrast),
                         MakeGammaValue(vtGamma), GetSingleton<IGFX>(), true);
      GetSingleton<ICursor>()->Acquire(true);
      return true;
    }
  }
  else
  {
    if (IsInitialized())
    {
      if (IMainLoop *pML = GetSingleton<IMainLoop>()) pML->Pause(true, PAUSE_TYPE_INACTIVE);
      // restore default gamma correction
      SetGammaCorrection(0, 0, 0, GetSingleton<IGFX>(), true);
      GetSingleton<ICursor>()->Acquire(false);
      return true;
    }
  }
  return false;
}

bool STDCALL NMain::Initialize(HWND hWnd3D, HWND nWndInput, HWND hWndSound, bool bGame)
{
  // register all module object factories
  GetSLS()->AddFactory(GetMainObjectFactory());
  GetSLS()->AddFactory(GetImageObjectFactory());
  GetSLS()->AddFactory(GetInputObjectFactory());
  GetSLS()->AddFactory(GetGFXObjectFactory());
  GetSLS()->AddFactory(GetSFXObjectFactory());
  GetSLS()->AddFactory(GetAnimObjectFactory());
  GetSLS()->AddFactory(GetSceneObjectFactory());
  GetSLS()->AddFactory(GetAILogicObjectFactory());
  GetSLS()->AddFactory(GetStreamIOObjectFactory());
  GetSLS()->AddFactory(GetUIObjectFactory());
  GetSLS()->AddFactory(GetNetObjectFactory());
  GetSLS()->AddFactory(GetMissionObjectFactory());
  // initialize cached global random generator pointer (must be after singleton
  // registry is populated, not during static init — see Phase 6.5 of SingletonPlan)
  g_pGlobalRandomGen = GetSingleton<IRandomGen>();
  // files inspector
  {
    IFilesInspector *pFI = CreateObject<IFilesInspector>(MAIN_FILES_INSPECTOR);
    RegisterSingleton(IFilesInspector::tidTypeID, pFI);
  }
  // create and register image processing
  {
    CPtr<IImageProcessor> pIP = CreateObject<IImageProcessor>(IMAGE_PROCESSOR);
    RegisterSingleton(IImageProcessor::tidTypeID, pIP);
  }
  // create and register game timer
  {
    IGameTimer *pTimer = CreateObject<IGameTimer>(MAIN_GAME_TIMER);// CreateGameTimer();
    RegisterSingleton(IGameTimer::tidTypeID, pTimer);
    pTimer->Init();
  }
  // create and init input
  {
    CPtr<IInput> pInput = CreateObject<IInput>(INPUT_INPUT);
    RegisterSingleton(IInput::tidTypeID, pInput);
    pInput->Init(nWndInput);
  }
  // create and init graphics
  {
    // apply GFX video memory limits (previously called by CModuleLoadAutoMagic)
    CGFXModuleChecker gfxChecker;
    gfxChecker.SetModuleFunctionalityLimits();
    //
    CPtr<IGFX> pGFX = CreateObject<IGFX>(GFX_GFX);
    if (pGFX->Init(nullptr, hWnd3D) != true) return false;
    RegisterSingleton(IGFX::tidTypeID, pGFX);// register GFX to singleton
    // GFX managers
    CPtr<ITextureManager> pTM = CreateObject<ITextureManager>(GFX_TEXTURE_MANAGER);
    RegisterSingleton(ITextureManager::tidTypeID, pTM);// register texture manager to singleton
    pTM->Init();
    CPtr<IMeshManager> pMM = CreateObject<IMeshManager>(GFX_MESH_MANAGER);
    RegisterSingleton(IMeshManager::tidTypeID, pMM);// register mesh manager to singleton
    pMM->Init();
    //
    CPtr<IFontManager> pFM = CreateObject<IFontManager>(GFX_FONT_MANAGER);
    RegisterSingleton(IFontManager::tidTypeID, pFM);// register font manager to singleton
    pFM->Init();
  }
  // create and init sound
  {
    CPtr<ISFX> pSFX = CreateObject<ISFX>(SFX_SFX);
    RegisterSingleton(ISFX::tidTypeID, pSFX);// register SFX to singleton
    pSFX->Init(hWndSound, 0, SFX_OUTPUT_DSOUND, 44100, 32);
    pSFX->SetDistanceFactor(fWorldCellSize / 2.0f);
    pSFX->SetRolloffFactor(GetGlobalVar("Sound.RolloffFactor", 1.0f));
    //
    CPtr<ISoundManager> pSM = CreateObject<ISoundManager>(SFX_SOUND_MANAGER);
    RegisterSingleton(ISoundManager::tidTypeID, pSM);// register sound manager to singleton
    pSM->Init();
  }
  // create animation manager
  {
    CPtr<IAnimationManager> pAM = CreateObject<IAnimationManager>(ANIM_ANIMATION_MANAGER);
    RegisterSingleton(IAnimationManager::tidTypeID, pAM);// register animation manager to singleton
    pAM->Init();
  }
  // create camera & cursor
  {
    // camera
    CPtr<ICamera> pCamera = CreateObject<ICamera>(SCENE_CAMERA);
    pCamera->Init(GetSingletonGlobal());
    RegisterSingleton(ICamera::tidTypeID, pCamera);// register camera to singleton
    // cursor
    CPtr<ICursor> pCursor = CreateObject<ICursor>(SCENE_CURSOR);
    RegisterSingleton(ICursor::tidTypeID, pCursor);// register cursor to singleton
    pCursor->Init(GetSingletonGlobal());
    pCursor->SetPos(0, 0);
    // particles
    CPtr<IParticleManager> pPM = CreateObject<IParticleManager>(PFX_MANAGER);
    RegisterSingleton(IParticleManager::tidTypeID, pPM);// register ParticleManger to singleton
    pPM->Init();
  }
  // create AI part
  {
    CPtr<IAILogic> pAILogic = CreateObject<IAILogic>(AI_LOGIC);
    RegisterSingleton(IAILogic::tidTypeID, pAILogic);
    const_cast<CDifficultyLevel *>(pAILogic->GetDifficultyLevel())->Init();

    CPtr<IAIEditor> pAIEditor = CreateObject<IAIEditor>(AI_EDITOR);
    RegisterSingleton(IAIEditor::tidTypeID, pAIEditor);
  }
  // create and init text managing system
  {
    CPtr<ITextManager> pTextMan = CreateObject<ITextManager>(TEXT_MANAGER);
    RegisterSingleton(ITextManager::tidTypeID, pTextMan);
    pTextMan->Init();
    pTextMan->AddTextFile("textes\\strings.txt");
    pTextMan->AddTextFile("textes\\tooltips.txt");
    pTextMan->AddTextFile("textes\\acks.txt");
  }
  //
  // create scene objects
  {
    // scene
    CPtr<IScene> pScene = CreateObject<IScene>(SCENE_SCENE);
    pScene->Init(GetSingletonGlobal());
    RegisterSingleton(IScene::tidTypeID, pScene);// register scene graph to singleton
    // vis obj builder
    CPtr<IVisObjBuilder> pVOB = CreateObject<IVisObjBuilder>(SCENE_VISOBJ_BUILDER);
    pVOB->Init(GetSingletonGlobal());
    RegisterSingleton(IVisObjBuilder::tidTypeID, pVOB);
  }
  // create and init UI system
  {
    CPtr<IMaskManager> pMM = CreateObject<IMaskManager>(MASK_MANAGER);
    RegisterSingleton(IMaskManager::tidTypeID, pMM);
    pMM->Init();
  }

  // create commands history object
  {
    ICommandsHistory *pHistory = CreateObject<ICommandsHistory>(MAIN_COMMANDS_HISTORY_INTERNAL);
    RegisterSingleton(ICommandsHistory::tidTypeID, pHistory);
  }
  // create single-player transceiver
  {
    ITransceiver *pTransceiver = CreateObject<ITransceiver>(MAIN_SP_TRANSCEIVER);
    pTransceiver->Init(GetSingletonGlobal(), -1);
    RegisterSingleton(ITransceiver::tidTypeID, pTransceiver);
  }
  // create mission objects
  {
    IClientAckManager *pAckMan = CreateObject<IClientAckManager>(IClientAckManager::tidTypeID);
    pAckMan->Init();
    RegisterSingleton(IClientAckManager::tidTypeID, pAckMan);
    IMPToUICommandManager *pMPToUI = CreateObject<IMPToUICommandManager>(IMPToUICommandManager::tidTypeID);
    RegisterSingleton(IMPToUICommandManager::tidTypeID, pMPToUI);

    IMessageLinkContainer *pMessageLink = CreateObject<IMessageLinkContainer>(IMessageLinkContainer::tidTypeID);
    pMessageLink->Init();
    RegisterSingleton(IMessageLinkContainer::tidTypeID, pMessageLink);
  }
  // scenario tracker
  {
    IScenarioTracker *pST = CreateObject<IScenarioTracker>(MAIN_SCENARIO_TRACKER);
    RegisterSingleton(IScenarioTracker::tidTypeID, pST);
    IUserProfile *pUserProfile = CreateObject<IUserProfile>(MAIN_USER_PROFILE);
    RegisterSingleton(IUserProfile::tidTypeID, pUserProfile);
  }
  bInitialized = true;
  return true;
}

bool STDCALL NMain::IsInitialized() { return bInitialized; }

bool STDCALL NMain::Finalize()
{
  GetSingleton<ISFX>()->Done();
  GetSingleton<ITransceiver>()->Done();

  GetSingletonGlobal()->Done();
  //
  return false;
}

bool STDCALL NMain::CanLaunch()
{
#if defined(_DO_ASSERT) || defined(_DO_ASSERT_SLOW)
  // check for remote drive - not allowed to run from!
  char buffer[2048];
  GetModuleFileName(nullptr, buffer, 2048);
  std::string szModuleDir = buffer;
  szModuleDir.erase(szModuleDir.find('\\'));
  if (szModuleDir.empty()) return true;
  if (szModuleDir[szModuleDir.size() - 1] != '\\') szModuleDir += '\\';
  //
  if (GetDriveType(szModuleDir.c_str()) == DRIVE_REMOTE)
  {
    MessageBoxA(nullptr, "Program can't be run from the remote drive!", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    // MessageBox( 0, "That's it!", "ERROR", MB_OK | MB_ICONEXCLAMATION );
    return false;
  }

#endif // defined(_DO_ASSERT) || 
  return true;
}