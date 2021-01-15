/*! @file	pdv.h
*
*  @warning This is the internal header of the ODP project.
*  Do not use it directly in other code. Please note that this file
*  is based on the open source code from
*  <a href="https://github.com/achu6393/dynamicWeightedClustering">
*	dynamicWeightedClustering
*  </a>
*  Copyright (C) Qiuchen Qian, 2020
*  Imperial College, London
*/


#pragma once
#include "sensornode.h"

#ifndef PDV_H_
#define PDV_H_

/*!
*  @class		PDV pdv.h "pdv.h"
*  @brief		Implementation of @a PDV class
*
*  The @a PDV class includes basic attributes like PDV position, flight time
*  and energy. Some PDV actions like fly approaching to next target node through
*  GPS localization and UWB localization, return-to-home process, inductive
*  power transfer and flight simulation are included as well.
*
*  @author		Qiuchen Qian
*  @version	5
*  @date		2020
*  @copyright	MIT Public License
*/
template <class T>
class PDV : protected SensorNode<T>
{
	//! @publicsection
public:
	Point<T> pos;	/*!< A @a Point object with data type @c T for sensor node position [m]*/
	double f_time = 0.;			/*!< PDV flight time [h]*/
	double f_eng = 187.;			/*!< PDV remain energy [Wh]*/
	double f_dist = 0.;			/*!< PDV flight distance [m]*/

	//! A default constructer with coordinate(0, 0), 0 flight time and full energy.
	PDV();
	//! A default destructer which sets PDV attributes to initial state.
	~PDV();

	//! @brief		Reset PDV status to initial value
	void resetPdvStatus();

	double getPdvSpeed() const { return this->f_speed; }

	/*! @brief			Check if the number of requested sensor nodes is larger than @a MIN_REQUESTS .
	*   @param sn_list	A vector of all sensor nodes.
	*   @return			If true, the PDV can fly for one task.
	*				If false, end the process.
	*/
	bool taskCheck(std::vector<SensorNode<T>>& sn_list);

	void updatePdvStatus(const Point<T>& p);

	/*! @brief			Calculate energy consumption according to power and spent time.
	*
	*   Related formula: Energy E = P * t
	*
	*   @param t		Spent time [h].
	*   @return			the consumed energy [Wh].
	*/
	double calcEnergyCost(const double& t) const { return this->pdv_power * (t + 5.6e-3); }

	/*! @brief			Calculate energy consumption after inductive power transfer.
	*
	*   Related formula: Energy E = 1 / (2 * n_{rf2dc} * 3600) * C * (V_{max} - V)^2
	*
	*   @param next_sn	The next target sensor node to charge
	*   @param e		Energy variable to be updated [Wh].
	*/
	void iptEnergyCost(const SensorNode<T>& next_sn, double& e);

	/*! @brief			Update @a flight_time_ according to @a t .
	*   @param t		Spent time [h].
	*/
	void updateFlightTime(const double& t) { this->flight_time_ += t; }

	/*! @brief			Update @a flight_time_ according to @a t1 and @a t2 .
	*   @param t1		Spent time [h].
	*   @param t2		Spent time [h].
	*/
	void updateFlightTime(const double& t1, const double& t2) { this->flight_time_ += (t1 + t2); }

	/*! @brief			Update @a pdv_energy_ according to @a e .
	*   @param e		Energy consumption [Wh]
	*/
	void updateEnergy(const double& e) { this->pdv_energy_ -= e; }

	/*! @brief			Update @a pdv_energy_ according to @a e1 and @a e2 .
	*   @param e1		Energy consumption [Wh]
	*   @param e2		Energy consumption [Wh]
	*/
	void updateEnergy(const double& e1, const double& e2) { this->pdv_energy_ -= (e1 + e2); }

	/*! @brief			Update @a flight_distance_ according to @a d .
	*   @param d		Flight distance [m]
	*/
	void updateFlightDist(const double& d) { this->flight_distance_ += d; }

	/*! @brief			Simulate the whole process.
	*
	*   The PDV will depart from the base station and visit the sensor nodes in @a path
	*   one by one, and once it completed all targets in the recharging list, it will
	*   return to the base station. Everytime before the PDV visiting the next target
	*   sensor node, it will check its remain energy, if not enough for next visit, power
	*   transfer and RTH implementation, it execute RTH immediately.
	*
	*   @param charged_e	Total charged energy to be recorded.
	*   @param pdv_t		Total flight time of the PDV.
	*   @param sn_list	A vector of all sensor nodes
	*   @param path		A vector of all points to visit
	*   @return			Task achievement percenetage [%]
	*/
	float flightSimulation(double& charged_e, double& pdv_t, std::vector<SensorNode<T>>& sn_list, std::vector<Point<T>>& path);

	float singleStageFlight(double& charged_e, double& pdv_t, std::vector<SensorNode<T>>& sn_list, std::vector<Point<T>>& path);

	//! @privatesection
private:
	int min_charge_num = 20;			/*!< Minimum number of sensor nodes to charge*/
	int flight_altitude = 20;			/*!< PDV flight altitude [m]*/
	double pdv_power = 363.888;			/*!< PDV power rating [W]*/
	double f_speed = 2.16e4;			/*!< Maximum approaching speed through GPS localization [m/h]*/
};

#endif // !PDV_H_
