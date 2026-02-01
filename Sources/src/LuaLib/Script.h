// ////
#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class Script
{
public:
  using CFunction = lua_CFunction;

  //
  /* *
     */
  class Object
  {
  public:
    /* *
       */
    Object(const Object &src) : m_parent(src.m_parent) { m_stackIndex = src.m_stackIndex; }

    /* *
       */
    const Object &operator=(const Object &src)
    {
      m_stackIndex = src.m_stackIndex;
      return *this;
    }

    Script &GetParent() const { return m_parent; }
    lua_State *GetState() const { return m_parent.m_state; }

    int GetType() const { return lua_type(GetState(), m_stackIndex); }

    bool IsNil() const { return m_parent.IsNil(m_stackIndex); }
    bool IsTable() const { return m_parent.IsTable(m_stackIndex); }
    bool IsUserData() const { return m_parent.IsUserData(m_stackIndex); }
    bool IsCFunction() const { return lua_iscfunction(GetState(), m_stackIndex) != 0; }
    bool IsNumber() const { return lua_isnumber(GetState(), m_stackIndex) != 0; }
    bool IsString() const { return m_parent.IsString(m_stackIndex); }
    bool IsFunction() const { return m_parent.IsFunction(m_stackIndex); }
    bool IsNull() const { return m_parent.IsNull(m_stackIndex); }

    int GetStackIndex() const { return m_stackIndex; }

    int GetInteger() const { return static_cast<int>(lua_tonumber(GetState(), m_stackIndex)); }
    operator int() const { return GetInteger(); }
    float GetNumber() const { return static_cast<float>(lua_tonumber(GetState(), m_stackIndex)); }
    const char *GetString() const { return lua_tostring(GetState(), m_stackIndex); }
    operator const char *() const { return GetString(); }
    size_t StrLen() const { return lua_rawlen(GetState(), m_stackIndex); }
    CFunction GetCFunction() const { return lua_tocfunction(GetState(), m_stackIndex); }
    void *GetUserData() const { return lua_touserdata(GetState(), m_stackIndex); }
    const void *GetPointer() const { return lua_topointer(GetState(), m_stackIndex); }

    /* *
       */
    Object CreateTable(const char *name)
    {
      int val;
      val = m_parent.GetTop();
      lua_newtable(GetState());// T
      val = m_parent.GetTop();
      lua_pushstring(GetState(), name);// T name
      val = m_parent.GetTop();
      lua_pushvalue(GetState(), lua_gettop(GetState()) - 1);// Tname T
      val = m_parent.GetTop();
      lua_settable(GetState(), m_stackIndex);
      val = m_parent.GetTop();

      return Object(m_parent, m_parent.GetTop());
    }

    /* *
       */
    void SetNumber(const char *name, double value)
    {
      lua_pushstring(GetState(), name);
      lua_pushnumber(GetState(), value);
      lua_settable(GetState(), m_stackIndex);
    }

    /* *
       */
    void SetString(const char *name, const char *value)
    {
      lua_pushstring(GetState(), name);
      lua_pushstring(GetState(), value);
      lua_settable(GetState(), m_stackIndex);
    }

    /* *
       */
    void SetUserData(const char *name, void *data)
    {
      lua_pushstring(GetState(), name);
      lua_pushlightuserdata(GetState(), data);
      lua_settable(GetState(), m_stackIndex);
    }

    /* *
       */
    Object GetByName(const char *name)
    {
      lua_pushstring(GetState(), name);
      lua_rawget(GetState(), m_stackIndex);
      return Object(m_parent, m_parent.GetTop());
    }

    /* *
       */
    Object GetByIndex(int index)
    {
      lua_rawgeti(GetState(), m_stackIndex, index);
      return Object(m_parent, m_parent.GetTop());
    }

  protected:
    friend class Script;

    Object(Script &parent, int index) :
      m_parent(parent), m_stackIndex(index) {}

    Script &m_parent;// !< The parent script of this object.
    int m_stackIndex;// !< The stack index representing this object.
  };

  //
  //
  //
  /* *
   */
  class AutoBlock
  {
  public:
    AutoBlock(Script &script) :
      m_script(script) { m_stackTop = m_script.GetTop(); }

    AutoBlock(Object &object) :
      m_script(object.GetParent()) { m_stackTop = m_script.GetTop(); }

    ~AutoBlock() { m_script.SetTop(m_stackTop); }

  private:
    AutoBlock(const AutoBlock &src);// Not implemented
    const AutoBlock &operator=(const AutoBlock &src);// Not implemented

    Script &m_script;
    int m_stackTop;
  };


  //
  enum { NOREF = LUA_NOREF };

  enum { REFNIL = LUA_REFNIL };

  Script(bool initStandardLibrary = true);
  Script(lua_State *state);
  ~Script();
  void Init(bool initStandardLibrary = true);
  void Clear();

  // Basic stack manipulation.
  int GetTop() { return lua_gettop(m_state); }
  void SetTop(int index) { lua_settop(m_state, index); }
  void PushValue(int index) { lua_pushvalue(m_state, index); }
  void Remove(int index) { lua_remove(m_state, index); }
  void Insert(int index) { lua_insert(m_state, index); }
  bool EnsureStack(int n) { return lua_checkstack(m_state, n) != 0; }

  Object GetObject(int index) { return Object(*this, index); }
  void PushObject(Object object) { lua_pushvalue(m_state, object.GetStackIndex()); }

  // access functions (stack -> C)
  int Equal(int index1, int index2) { return lua_compare(m_state, index1, index2, LUA_OPEQ); }
  int LessThan(int index1, int index2) { return lua_compare(m_state, index1, index2, LUA_OPLT); }

  // push functions (C -> stack)
  void PushBool(bool value)
  {
    if (value) lua_pushnumber(m_state, 1);
    else lua_pushnil(m_state);
  }

  void PushNil() { lua_pushnil(m_state); }
  void PushNumber(double n) { lua_pushnumber(m_state, n); }
  void PushLString(const char *s, size_t len) { lua_pushlstring(m_state, s, len); }
  void PushString(const char *s) { lua_pushstring(m_state, s); }
  void PushCClosure(lua_CFunction fn, int n) { lua_pushcclosure(m_state, fn, n); }
  void PushLightUserData(void *u) { lua_pushlightuserdata(m_state, u); }

  // get functions (Lua -> stack)
  Object GetGlobal(const char *name)
  {
    lua_getglobal(m_state, name);
    return Object(*this, GetTop());
  }

  void GetTable(int index) { lua_gettable(m_state, index); }
  void RawGet(int index) { lua_rawget(m_state, index); }
  void RawGetI(int index, int n) { lua_rawgeti(m_state, index, n); }

  Object GetGlobals()
  {
    lua_pushglobaltable(m_state);
    return Object(*this, GetTop());
  }

  void GetRef(int ref) { lua_rawgeti(m_state, LUA_REGISTRYINDEX, ref); }

  Object NewTable()
  {
    lua_newtable(m_state);
    return Object(*this, GetTop());
  }

  // set functions(stack -> Lua)
  void SetGlobal(const char *name) { lua_setglobal(m_state, name); }
  void SetTable(int index) { lua_settable(m_state, index); }
  void RawSet(int index) { lua_rawset(m_state, index); }
  void RawSetI(int index, int n) { lua_rawseti(m_state, index, n); }
  int Ref() { return luaL_ref(m_state, LUA_REGISTRYINDEX); }


  // "do" functions(run Lua code)
  int Call(int nargs, int nresults) { return lua_pcall(m_state, nargs, nresults, 0); }
  void RawCall(int nargs, int nresults) { lua_call(m_state, nargs, nresults); }
  int DoFile(const char *filename) { return luaL_dofile(m_state, filename); }
  int DoString(const char *str) { return luaL_dostring(m_state, str); }
  int DoBuffer(const char *buff, size_t size, const char *name) { return luaL_loadbuffer(m_state, buff, size, name) || lua_pcall(m_state, 0, LUA_MULTRET, 0); }


  // miscellaneous functions
  void Error(const char *s) { lua_pushstring(m_state, s); lua_error(m_state); }

  void Unref(int ref) { luaL_unref(m_state, LUA_REGISTRYINDEX, ref); }

  int Next(int index) { return lua_next(m_state, index); }
  lua_Unsigned GetN(int index) { return lua_rawlen(m_state, index); }

  void Concat(int n) { lua_concat(m_state, n); }

  // Helper function
  void Pop(int amount = 1) { lua_pop(m_state, amount); }

  struct SRegFunction
  {
    const char *name;
    CFunction func;
  };

  void Register(const SRegFunction *pList);
  void Register(const char *funcName, lua_CFunction function) { lua_register(m_state, funcName, function); }
  void PushUserData(void *u) { lua_pushlightuserdata(m_state, u); }
  void PushCFunction(lua_CFunction f) { lua_pushcclosure(m_state, f, 0); }

  bool IsFunction(int index) { return lua_isfunction(m_state, index) != 0; }
  bool IsCFunction(int index) { return lua_iscfunction(m_state, index) != 0; }
  bool IsString(int index) { return lua_isstring(m_state, index) != 0; }
  bool IsNumber(int index) { return lua_isnumber(m_state, index) != 0; }
  bool IsTable(int index) { return lua_istable(m_state, index) != 0; }
  bool IsUserData(int index) { return lua_isuserdata(m_state, index) != 0; }
  bool IsNil(int index) { return lua_isnil(m_state, index) != 0; }
  bool IsNull(int index) { return lua_isnone(m_state, index) != 0; }

  int ConfigGetInteger(const char *section, const char *entry, int defaultValue = 0);
  float ConfigGetReal(const char *section, const char *entry, double defaultValue = 0.0);
  const char *ConfigGetString(const char *section, const char *entry, const char *defaultValue = "");
  void ConfigSetInteger(const char *section, const char *entry, int value);
  void ConfigSetReal(const char *section, const char *entry, double value);
  void ConfigSetString(const char *section, const char *entry, const char *value);

  void SaveText(const char *filename);

  operator lua_State *() { return m_state; }
  lua_State *GetState() const { return m_state; }

  friend class Object;

  lua_State *m_state;
  bool m_ownState;
};


/* *
 */
inline Script::Script(lua_State *state)
{
  m_state = state;
  m_ownState = false;
}


/* *
 */
inline void Script::Clear()
{
  // Only close the Lua state if we own it.
  if (m_ownState) lua_close(m_state);
}

//
inline Script::~Script() { Clear(); }