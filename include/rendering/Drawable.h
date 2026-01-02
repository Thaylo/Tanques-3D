/**
 * Drawable.h - Interface for renderable objects
 */

#ifndef DRAWABLE_H
#define DRAWABLE_H

/**
 * Abstract interface for objects that can be drawn/rendered.
 */
class Drawable {
public:
  virtual ~Drawable() {}
  virtual void draw() = 0;
};

#endif // DRAWABLE_H
