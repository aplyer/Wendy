///////////////////////////////////////////////////////////////////////
// Wendy demo system
// Copyright (c) 2007 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any
// purpose, including commercial applications, and to alter it and
// redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//     distribution.
//
///////////////////////////////////////////////////////////////////////
#ifndef WENDY_DEMOTIMELINE_H
#define WENDY_DEMOTIMELINE_H
///////////////////////////////////////////////////////////////////////

namespace wendy
{
  namespace demo
  {
  
///////////////////////////////////////////////////////////////////////

using namespace moira;

///////////////////////////////////////////////////////////////////////

class Timeline;

///////////////////////////////////////////////////////////////////////

/*! @ingroup demo
 */
class TimelineRuler : public UI::Widget
{
public:
  TimelineRuler(Timeline& timeline);
  Time getTimeElapsed(void) const;
  void setTimeElapsed(Time newTime);
  SignalProxy1<void, TimelineRuler&> getTimeChangedSignal(void);
private:
  void draw(void) const;
  void onButtonClicked(Widget& widget,
                       const Vector2& point,
		       unsigned int button,
		       bool clicked);
  void onDragMoved(Widget& widget, const Vector2& point);
  Signal1<void, TimelineRuler&> timeChangedSignal;
  Timeline& timeline;
  Time elapsed;
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup demo
 */
class TimelineTrack : public UI::Widget
{
public:
  TimelineTrack(Timeline& timeline, Effect& effect);
  Effect& getEffect(void) const;
private:
  enum DragMode
  {
    NOT_DRAGGING,
    DRAGGING_POSITION,
    DRAGGING_START,
    DRAGGING_DURATION,
  };
  void draw(void) const;
  void onButtonClicked(Widget& widget,
                       const Vector2& point,
		       unsigned int button,
		       bool clicked);
  void onDragBegun(Widget& widget, const Vector2& point);
  void onDragMoved(Widget& widget, const Vector2& point);
  void onDragEnded(Widget& widget, const Vector2& point);
  float getHandleSize(void) const;
  float getHandleOffset(void) const;
  Timeline& timeline;
  Effect& effect;
  DragMode mode;
  Ptr<UI::Menu> menu;
  float reference;
};

///////////////////////////////////////////////////////////////////////

/*! @ingroup demo
 */
class Timeline : public UI::Widget
{
public:
  Timeline(Show& show);
  bool createEffect(EffectType& type);
  void destroyEffect(void);
  Time getWindowStart(void) const;
  void setWindowStart(Time newStart);
  float getZoom(void) const;
  void setZoom(float newZoom);
  Time getTimeElapsed(void) const;
  void setTimeElapsed(Time newTime);
  Time getVisibleDuration(void) const;
  float getSecondWidth(void) const;
  Effect* getParentEffect(void) const;
  void setParentEffect(Effect* newEffect);
  SignalProxy1<void, Timeline&> getTimeChangedSignal(void);
private:
  typedef std::vector<TimelineTrack*> EffectList;
  void draw(void) const;
  void updateScroller(void);
  void onValueChanged(UI::Scroller& scroller);
  void onTimeChanged(TimelineRuler& ruler);
  void onAreaChanged(UI::Widget& widget);
  Signal1<void, Timeline&> timeChangedSignal;
  Show& show;
  EffectList tracks;
  Effect* parent;
  Time start;
  float zoom;
  unsigned int effectIndex;
  TimelineRuler* ruler;
  UI::Layout* trackLayout;
  UI::Scroller* timeScroller;
};

///////////////////////////////////////////////////////////////////////

  } /*namespace demo*/
} /*namespace wendy*/

///////////////////////////////////////////////////////////////////////
#endif /*WENDY_DEMOTIMELINE_H*/
///////////////////////////////////////////////////////////////////////
