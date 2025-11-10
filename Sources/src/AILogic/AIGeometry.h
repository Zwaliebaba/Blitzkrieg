#ifndef __AI_GEOMETRY_H__
#define __AI_GEOMETRY_H__

#pragma once
// //////////////////////////////////////////////////////////// 
// **********************************************************************
// * Geometry for AI : vectors, lines *
// **********************************************************************
// //////////////////////////////////////////////////////////// 
const auto V3_CAMERA_HOR = CVec3(-FP_SQRT_2 / 2.0f, FP_SQRT_2 / 2.0f, 0.0f);// V3_AXIS_Y - V3_AXIS_X
// //////////////////////////////////////////////////////////// 
// **********************************************************************
// *CVector*
// **********************************************************************
// //////////////////////////////////////////////////////////// 
struct SVector
{
  int x, y;
  //
  SVector() {}
  SVector(const int _x, const int _y) : x(_x), y(_y) {}
  SVector(const SVector &vec) : x(vec.x), y(vec.y) {}
  SVector(const CVec2 &vec) : x(vec.x), y(vec.y) {}

  const CVec2 ToCVec2() const { return CVec2(x, y); }

  const CVec2 Norm() const
  {
    if (float revR = sqrt(x * x + y * y))
    {
      revR = 1 / revR;
      return CVec2(static_cast<float>(x) * revR, static_cast<float>(y) * revR);
    }
    return VNULL2;
  }

  //
  void TurnLeft()
  {
    std::swap(x, y);
    x = -x;
  }

  void TurnRight()
  {
    std::swap(x, y);
    y = -y;
  }

  void TurnLeftUntilAxis()
  {
    const short int signX = Sign(x);
    const short int signY = Sign(y);
    /* if ( signX >= 0 && signY == -1 ) { x = 1;   */
    if (signX != 0 && signX * signY >= 0) x = 0, y = signX;
    else x = -signY, y = 0;
  }

  void TurnLeftUntil45()
  {
    const short int signX = Sign(x);
    const short int signY = Sign(y);

    if (signX != 0 && signX * signY >= 0)
    {
      if (abs(x) > abs(y)) x = signX;
      else x = 0;
      y = signX;
    }
    else
    {
      if (abs(y) > abs(x)) y = signY;
      else y = 0;

      x = -signY;
    }
  }

  void TurnRightUntilAxis()
  {
    const short int signX = Sign(x);
    const short int signY = Sign(y);
    /* if ( signX >=0 && signY == 1 ) { x = 1;   */
    if (signX != 0 && signX * signY <= 0) x = 0, y = -signX;
    else y = 0, x = signY;
  }

  void TurnRightUntil45()
  {
    const short int signX = Sign(x);
    const short int signY = Sign(y);

    if (signX != 0 && signX * signY <= 0)
    {
      if (abs(x) > abs(y)) x = signX;
      else x = 0;
      y = -signX;
    }
    else
    {
      if (abs(y) > abs(x)) y = signY;
      else y = 0;
      x = signY;
    }
  }

  const bool operator!=(const SVector &vec) const { return x != vec.x || y != vec.y; }
  const bool operator==(const SVector &vec) const { return x == vec.x && y == vec.y; }
  const SVector operator-(const SVector &vec) const { return SVector(x - vec.x, y - vec.y); }
  const SVector operator+(const SVector &vec) const { return SVector(x + vec.x, y + vec.y); }

  SVector &operator+=(const SVector &vec)
  {
    x += vec.x;
    y += vec.y;
    return *this;
  }

  SVector &operator-=(const SVector &vec)
  {
    x -= vec.x;
    y -= vec.y;
    return *this;
  }

  const int operator*(const SVector &vec) const { return x * vec.x + y * vec.y; }

  SVector &operator*=(const int n)
  {
    x *= n;
    y *= n;
    return *this;
  }

  // complete division
  SVector &operator/=(const int n)
  {
    const float coeff = 1.0f / static_cast<float>(n);
    x *= coeff;
    y *= coeff;
    return *this;
  }
};

// //////////////////////////////////////////////////////////// 
// only with fairly close points
inline long SquareOfDistance(const SVector &v1, const SVector &v2) { return square(static_cast<long>(v1.x - v2.x)) + square(static_cast<long>(v1.y - v2.y)); }
// //////////////////////////////////////////////////////////// 
inline float SquareOfDistance(const CVec2 &v1, const CVec2 &v2) { return square(v1.x - v2.x) + square(v1.y - v2.y); }
// //////////////////////////////////////////////////////////// 
inline bool IsAlmostZero(CVec2 vec)
{
  static constexpr float eps = 0.0001f;
  return (fabs(vec.x) < eps && fabs(vec.y) < eps);
}

// //////////////////////////////////////////////////////////// 
inline bool IsAlmostZero(const float x, const float y)
{
  static constexpr float eps = 0.0001f;
  return (fabs(x) < eps && fabs(y) < eps);
}

// //////////////////////////////////////////////////////////// 
inline int mDistance(const SVector &vec1, const SVector &vec2) { return Max(abs(vec1.x - vec2.x), abs(vec1.y - vec2.y)); }
// //////////////////////////////////////////////////////////// 
inline CVec2 Project(const CVec2 &vec, const CVec2 &axis)
{
  if (axis == VNULL2) return VNULL2;
  return axis * ((vec * axis) / fabs2(axis));
}

// //////////////////////////////////////////////////////////// 
inline float DoubleTrSquare(const CVec2 &side1, const CVec2 &side2) { return side1.x * side2.y - side2.x * side1.y; }
// //////////////////////////////////////////////////////////// 
inline CVec2 Norm(const CVec2 &vec)
{
  if (vec == VNULL2) return VNULL2;
  return vec / fabs(vec);
}

// //////////////////////////////////////////////////////////// 
// **********************************************************************
// *SLine*
// **********************************************************************
// //////////////////////////////////////////////////////////// 
struct SLine
{
  int a, b, c;

  SLine() {};
  SLine(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {}

  SLine(const SVector &ptStart, const SVector &ptFinish)
    : a(ptFinish.y - ptStart.y), b(ptStart.x - ptFinish.x), c(ptFinish.x * ptStart.y - ptStart.x * ptFinish.y) {}

  bool IsPointOnLine(const SVector &point) const { return a * point.x + b * point.y + c == 0; }
  const int GetHPLineSign(const SVector &point) const { return Sign(a * point.x + b * point.y + c); }

  bool IsSegmIntersectLine(const SVector &ptStart, const SVector &ptFinish) const
  {
    const int t1 = Sign(GetHPLineSign(ptStart));
    const int t2 = Sign(GetHPLineSign(ptFinish));
    return (t1 >= 0) && (t2 <= 0) || (t1 <= 0) && (t2 >= 0);
  }

  const SLine GetPerpendicular(const SVector &point) const { return SLine(-b, a, b * point.x - a * point.y); }

  const SVector GetDirVector() const { return SVector(-b, a); }
};

// //////////////////////////////////////////////////////////// 
// **********************************************************************
// *CBres*
// **********************************************************************
// //////////////////////////////////////////////////////////// 
class CBres
{
  int xerr, yerr;
  int xlen, ylen, len;
  int xinc, yinc;

  SVector dir;
  //
  void Initialize()
  {
    xerr = 0;
    yerr = 0;

    xinc = Sign(xlen);
    yinc = Sign(ylen);
    xlen = abs(xlen) + 1;
    ylen = abs(ylen) + 1;
    len = Max(xlen, ylen);
  }

public:
  // to give directions
  void Init(const SVector &start, const SVector &finish)
  {
    xlen = finish.x - start.x;
    ylen = finish.y - start.y;

    Initialize();
  }

  // in order to produce points
  void InitPointByDirection(const SVector &start, const SVector &direction)
  {
    dir = start;
    xlen = direction.x;
    ylen = direction.y;

    Initialize();
  }

  // in order to produce points
  void InitPoint(const SVector &start, const SVector &finish)
  {
    dir = start;
    xlen = finish.x - start.x;
    ylen = finish.y - start.y;

    Initialize();
  }

  //
  void MakeStep()
  {
    xerr += xlen;
    if (xerr >= len) dir.x = xinc, xerr -= len;
    else dir.x = 0;

    yerr += ylen;
    if (yerr >= len) dir.y = yinc, yerr -= len;
    else dir.y = 0;
  }

  void MakePointStep()
  {
    xerr += xlen;
    if (xerr >= len) dir.x += xinc, xerr -= len;

    yerr += ylen;
    if (yerr >= len) dir.y += yinc, yerr -= len;
  }

  const SVector &GetDirection() const { return dir; }
};

// //////////////////////////////////////////////////////////// 
// **********************************************************************
// *SRect*
// **********************************************************************
// //////////////////////////////////////////////////////////// 
struct SRect
{
  union
  {
    struct
    {
      CVec2 v[4];
    };

    struct
    {
      CVec2 v1, v2, v3, v4;
    };
  };

  CVec2 dir, dirPerp, center;
  float lengthAhead, lengthBack, width;

  bool IsIntersectProject(const CVec2 &v1, const CVec2 &v2, const CVec2 &v3, const CVec2 &v4, const CVec2 &dir, float min, float max) const;

  // public:
  void InitRect(const CVec2 &_v1, const CVec2 &_v2, const CVec2 &_v3, const CVec2 &_v4);
  // half of the actual length and width is set (as if radii)
  void InitRect(const CVec2 &center, const CVec2 &dir, float length, float width);
  // the lengths forward and backward, half the width are specified
  void InitRect(const CVec2 &center, const CVec2 &dir, float lengthAhead, float lengthBack, float width);

  bool IsIntersected(const SRect &rect) const;

  // the boundaries of the rectangle do not belong to it
  bool IsPointInside(const CVec2 &point) const;
  bool IsIntersectCircle(const CVec2 &circleCenter, float r) const;
  bool IsIntersectCircle(const CCircle &circle) const { return IsIntersectCircle(circle.center, circle.r); }

  const int GetSide(WORD dirFromRectCenter) const;
  const int GetSide(const CVec2 &point) const;

  void Compress(float fFactor);
};

// //////////////////////////////////////////////////////////// 
const float fabs(SRect rect1, SRect rect2);
// the angle at which rect is visible from point
const WORD GetVisibleAngle(CVec2 point, SRect rect);
// //////////////////////////////////////////////////////////// 
// **********************************************************************
// *CBresZ*
// **********************************************************************
// //////////////////////////////////////////////////////////// 
class CBresZ
{
  int xerr, yerr;
  int xlen, ylen, len;
  int xinc, yinc, zinc, rinc;

  int intPart, dxy;

  SVector dir;
  int z, r;

public:
  void InitXY(const SVector &start, const SVector &finish)
  {
    dir = start;

    xlen = finish.x - start.x;
    ylen = finish.y - start.y;

    xerr = 0;
    yerr = 0;

    xinc = Sign(xlen);
    yinc = Sign(ylen);

    xlen = abs(xlen) + 1;
    ylen = abs(ylen) + 1;
    len = Max(xlen, ylen);

    NI_ASSERT_SLOW_T(xlen != 0 || ylen != 0, "Wrong line");
  }

  void InitZ(const int startZ, const int finishZ, const SVector &startPoint, const SVector &finishPoint)
  {
    NI_ASSERT_T(startPoint != finishPoint, "Wrong ray");

    const int zlen = finishZ - startZ;
    dxy = Max(abs(finishPoint.x - startPoint.x), abs(finishPoint.y - startPoint.y));

    intPart = zlen / dxy;
    rinc = abs(zlen % dxy);
    zinc = Sign(zlen);

    z = finishZ;
    r = 0;
  }

  void InitZWithStep(const int startZ, const int finishZ, const SVector &startPoint, const SVector &finishPoint)
  {
    NI_ASSERT_T(startPoint != finishPoint, "Wrong ray");

    const int zlen = finishZ - startZ;
    dxy = Max(abs(finishPoint.x - startPoint.x), abs(finishPoint.y - startPoint.y));

    intPart = zlen / dxy;
    rinc = abs(zlen % dxy);
    zinc = Sign(zlen);

    z = finishZ + intPart;
    r = rinc;
  }

  void MakeStep()
  {
    xerr += xlen;
    yerr += ylen;

    if (xerr >= len) dir.x += xinc, xerr -= len;
    if (yerr >= len) dir.y += yinc, yerr -= len;

    z += intPart;
    r += rinc;
    if (r >= dxy) z += zinc, r -= dxy;
  }

  const SVector &GetPoint() const { return dir; }
  const int GetZ() const { return z; }
};

// //////////////////////////////////////////////////////////// 
// **********************************************************************
// *CBSplne*
// **********************************************************************
// //////////////////////////////////////////////////////////// 
class CBSpline
{
  DECLARE_SERIALIZE;

  CVec2 a, b, c, d;
  CVec2 x, dx, d2x, d3x;
  CVec2 fw_dx, fw_d2x, fw_d3x;
  float t, tForward;
  BYTE cntToForward;

public:
  // to calculate spline
  const static float DELTA;
  // to look ahead for locked tiles along the way
  const static float DELTA_FORWARD;
  const static int N_OF_ITERATONS;
  const static int N_ITERS_TO_FORWARD;

  void Init(const CVec2 &p3, const CVec2 &p2, const CVec2 &p1, const CVec2 &p0);

  void Iterate();

  const CVec2 &GetPoint() const { return x; }
  const CVec2 &GetDX() const { return dx; }
  const float GetReverseR() const;

  struct SForwardIter
  {
    float t;
    CVec2 x;
    CVec2 fw_dx, fw_d2x, fw_d3x;
  };

  // if pIter->t == -1, then you cannot iterate further, because 
  const void StartForwardIterating(SForwardIter *pIter);
  const void IterateForward(SForwardIter *pIter);

  void DumpState() const;
};

// //////////////////////////////////////////////////////////// 
const WORD GetDirectionByVector(CVec2 vec);
const WORD GetDirectionByVector(float x, float y);
const CVec2 GetVectorByDirection(WORD dir);
const WORD GetZDirectionBy3DVector(float x, float y, float z);
const WORD GetZDirectionBy3DVector(const CVec2 &vec, float z);
// angle between vector and OXY
const WORD GetZAngle(float x, float y, float z);
// angle between vector and OXY
const WORD GetZAngle(const CVec2 &vec, float z);
const WORD DirsDifference(WORD dir1, WORD dir2);
const int DifferenceSign(WORD dir1, WORD dir2);
// in the corner from startAngleDir to finishAngleDir counterclockwise
bool IsInTheAngle(WORD dir, WORD startAngleDir, WORD finishAngleDir);
// dir in the minimum angle between dir1 and dir2
bool IsInTheMinAngle(WORD dir, WORD dir1, WORD dir2);
// minimum distance from a point to a segment
const float GetDistanceToSegment(const CVec2 &vSegmentStart, const CVec2 &vSegmentEnd, const CVec2 &vPoint);
// //////////////////////////////////////////////////////////// 
#endif // __AI_GEOMETRY_H__