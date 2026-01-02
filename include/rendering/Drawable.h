/**
 * Drawable.h - Base class for renderable entities
 *
 * Renderer-agnostic interface - draw() implemented by derived classes.
 */

#ifndef DRAWABLE_H
#define DRAWABLE_H

/**
 * Interface for drawable entities.
 * All drawable objects must implement draw().
 */
class Drawable {
public:
  virtual ~Drawable() = default;
  virtual void draw() = 0;
};

#endif // DRAWABLE_H
