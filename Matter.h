/*
 * Matter.h
 *
 *  Created on: 15/10/2012
 *      Author: thaylo
 */

#ifndef MATTER_H_
#define MATTER_H_

namespace std {

class Matter {
private:
	double mass;
	double charge;
public:
	Matter();
	double getMass() const;
	double getCharge() const;
	void setMass(double mass_);
	void setCharge(double charge_);
	virtual ~Matter() {}
};

} /* namespace std */
#endif /* MATTER_H_ */
