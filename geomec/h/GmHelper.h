#pragma once

namespace  gm{
  namespace  send{

    struct TMsg
    {
      HWND hWnd;
      UINT Msg;
      WPARAM wParam;
      LPARAM lParam;
    };

    void gen( const TMsg& d );
    void command( WPARAM wParam, LPARAM lParam=0 );
    void msg( UINT Msg, WPARAM wParam, LPARAM lParam=0 );
    void quit();
    void minimize();


    enum eStatusType{Text,ElapsedTime};
    void status( const QString& message, eStatusType type=Text );

    struct key_ext{ WORD key; int times; };
    void key_stroke(const std::vector<key_ext>& key_v);

    void key_stroke(const std::vector<WORD>& key_v);
    void key_stroke(WORD key);
    void key_stroke_ctrl(WORD key);

  } // namespace  send

  UINT cmd( const QString& key );

} // namespace  gm

