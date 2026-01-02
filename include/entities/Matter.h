/**
 * Matter.h - Physical matter properties
 */

#ifndef MATTER_H
#define MATTER_H

/**
 * Base class for entities with physical matter properties (mass, etc.)
 */
class Matter {
protected:
  double mass;

public:
  Matter();
  virtual ~Matter() {}

  double getMass() const;
  void setMass(double m);
};

#endif // MATTER_H
