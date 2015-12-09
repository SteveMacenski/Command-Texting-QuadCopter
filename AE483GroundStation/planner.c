//
// AE483GroundStation
// David Hanley
//
// planner.c
// This file contains all functions for planning. The most basic example
// would be to hold position at some desired (x,y,z,yaw).
//


/*Instructions AE483 Fall 2015:

- Look at the parameters and the variables defined below
- Using these, make changes to the PotentialField function at the bottom
*/

/*----------------------------------------------------------------------*/
/*------------------------------ Preamble ------------------------------*/
/*----------------------------------------------------------------------*/

/*--------------- Includes ---------------*/

//system
#include <math.h>

//planner
#include "planner.h"

//text message receiver
#include <stdio.h>
#include <string.h>




//Parameters
float param_katt = 30;
float param_batt = 0.5;
float param_krep = 0.35;
float param_brep = 0.35;
float param_kdescent = 1;
float param_bdescent = 0.5;

//Drone position and radius
float drone_q[3], drone_r = 0.2;

//Obstacle position and radius
float obst_q[3], obst_s = 0.2;

//Goal position
float goal_q[3] = {0, 0, 0};
float base_q[3] = {0, 0, 0};

//Vector from goal position to drone position
float v[3], v_norm;

//Vector from obstacle position to drone position
float vo[3], vo_norm;

//d and dgrad between drone and obstacle
float d, dgrad[3];

//Attractive, Repulsive and Total gradients
float gradfatt[3], gradfrep[3], gradf[3], gradf_norm;

//Gradient Descent
float dq[3], dqnorm;

/*------------- End Includes -------------*/

/*----------------------------------------------------------------------*/
/*------------------------------ Preamble ------------------------------*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*----------------------------- Functions ------------------------------*/
/*----------------------------------------------------------------------*/
/*--------------- Quat2Euler/Euler2Quat ---------------*/
void Planner_Quat2Euler_f(Quat_f *quat_param, Euler_f *euler_param)
{
	//variables
	float sqw = quat_param->qw*quat_param->qw;
	float sqx = quat_param->qx*quat_param->qx;
	float sqy = quat_param->qy*quat_param->qy;
	float sqz = quat_param->qz*quat_param->qz;
	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor

	//singularity tests
	float test = quat_param->qx*quat_param->qy + quat_param->qz*quat_param->qw;
	if (test > 0.499*unit) { // singularity at north pole
		euler_param->ty = 2 * atan2f(quat_param->qx, quat_param->qw);
		euler_param->tz = PI_f / 2;
		euler_param->tx = 0;
		return;
	}
	if (test < -0.499*unit) { // singularity at south pole
		euler_param->ty = -2 * atan2f(quat_param->qx, quat_param->qw);
		euler_param->tz = -PI_f / 2;
		euler_param->tx = 0;
		return;
	}

	//no singularity
	euler_param->ty = atan2f(2 * quat_param->qy*quat_param->qw - 2 * quat_param->qx*quat_param->qz, sqx - sqy - sqz + sqw);
	euler_param->tz = asinf(2 * test / unit);
	euler_param->tx = atan2f(2 * quat_param->qx*quat_param->qw - 2 * quat_param->qy*quat_param->qz, -sqx + sqy - sqz + sqw);
}
/*------------- End Quat2Euler/Euler2Quat -------------*/

/*----- MakeState -----*/
void Planner_MakeState6f_PosQuat(State6_f *state_param, Pos_f *pos_param, Quat_f *quat_param)
{
	state_param->Pos.x = pos_param->x;
	state_param->Pos.y = pos_param->y;
	state_param->Pos.z = pos_param->z;
	Planner_Quat2Euler_f(quat_param, &(state_param->Ori));
}
/*--- End MakeState ---*/
/*-- Potential Field --*/


int switchcount = 0;
int firsttime = 1;
float prev_time = 0, delta_t;

// MAKE YOUR EDITS BELOW HERE

void PotentialField(State6_f state_param,State6_f ObstCurrent, PosYaw_f *dGoal, float time, int PotFlag)
{
	if(PotFlag == 1){
	char L[] = "L";
	char R[] = "R";
	char U[] = "U";
	char D[] = "D";
	char F[] = "F";
	char B[] = "B";
	char C[] = "C";
	char G[] = "G";
	char N[] = "N";
	char Z[] = "Z";

	char str[2];
	float dist = 0.50;
	double graddist = sqrt((base_q[0]-goal_q[0])*(base_q[0]-goal_q[0])+(base_q[1]-goal_q[1])*(base_q[1]-goal_q[1])+(base_q[2]-goal_q[2])*(base_q[2]-goal_q[2]));
	FILE *fp;
	fp = fopen("C:\\Python27\\Command-Texting-QuadCopter\\AE483GroundStation\\commands.txt" , "r");

	fgets(str, 2, fp);

	fclose(fp);
	
	if (strcmp(str,N) != 0){
		FILE *fp = fopen("C:\\Python27\\Command-Texting-QuadCopter\\AE483GroundStation\\commands.txt","w+");
		fprintf(fp,"N");
		fclose(fp);
	}

	/*---------- Simple Desired Position Set ------------------*/
	
		if (strcmp(str,L) == 0){
			goal_q[0]=goal_q[0];
			goal_q[1]=goal_q[1]+dist;
			goal_q[2]=goal_q[2];
        	//printf("command is left, do stuff\n");
	}
        else if (strcmp(str,R) == 0)
		{
			goal_q[0]=goal_q[0];
			goal_q[1]=goal_q[1]-dist;
			goal_q[2]=goal_q[2];
       		//printf("command is right do stuff\n");
	}
        else if (strcmp(str,U) == 0)
		{
			goal_q[0]=goal_q[0];
			goal_q[1]=goal_q[1];
			goal_q[2]=goal_q[2]-dist;
       		//printf("command is up do stuff\n");
	}
        else if (strcmp(str,D) == 0)
		{
			goal_q[0]=goal_q[0];
			goal_q[1]=goal_q[1];
			goal_q[2]=goal_q[2]+dist;
       		//printf("command is down do stuff\n");
	}
        else if (strcmp(str,F) == 0)
		{
			goal_q[0]=goal_q[0]+dist;
			goal_q[1]=goal_q[1];
			goal_q[2]=goal_q[2];
			//printf("command is forward do stuff\n");
		}
        else if (strcmp(str,B) == 0)
		{
			goal_q[0]=goal_q[0]-dist;
			goal_q[1]=goal_q[1];
			goal_q[2]=goal_q[2];
			//printf("command is back do stuff\n");
		}

        else if (strcmp(str,C) == 0)
		{
       		//printf("command is clean do stuff\n");
	}
        else if (strcmp(str,G) == 0)
		{
       		//printf("command is GPS go to coordinates\n");
	}
		else if (strcmp(str,Z) == 0)
		{	goal_q[0]=base_q[0];
			goal_q[1]=base_q[1];
			goal_q[2]=base_q[2];
			while (graddist >= dist)
			{
				drone_q[0] = state_param.Pos.x;		drone_q[1] = state_param.Pos.y;		drone_q[2] = state_param.Pos.z;
				obst_q[0] = ObstCurrent.Pos.x;		obst_q[1] = ObstCurrent.Pos.y;		obst_q[2] = ObstCurrent.Pos.z;

				delta_t = time - prev_time;

				//Get attractive gradient:
				v[0] = drone_q[0]-goal_q[0]; v[1] = drone_q[1]-goal_q[1]; v[2] = drone_q[2]-goal_q[2];
				v_norm = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	
				//Enter your code to compute Attractive Gradient here

				if (v_norm<=param_batt)
				{
					gradfatt[0] = param_katt*(drone_q[0]-goal_q[0]);
					gradfatt[1] = param_katt*(drone_q[1]-goal_q[1]);
					gradfatt[2] = param_katt*(drone_q[2]-goal_q[2]);
																		}
				else
				{
					gradfatt[0] = param_katt*param_batt*((drone_q[0]-goal_q[0])/v_norm);
					gradfatt[1] = param_katt*param_batt*((drone_q[1]-goal_q[1])/v_norm);
					gradfatt[2] = param_katt*param_batt*((drone_q[2]-goal_q[2])/v_norm);
																							}

		
				//Get repulsive gradient:
				gradfrep[0] = 0;	gradfrep[1] = 0;	gradfrep[2] = 0;
		
				vo[0] = drone_q[0]-obst_q[0]; vo[1] = drone_q[1]-obst_q[1]; vo[2] = drone_q[2]-obst_q[2];
				vo_norm = sqrt(vo[0]*vo[0] + vo[1]*vo[1] + vo[2]*vo[2]);
		
				//Enter code to compute Repulsive Gradient here


				d = vo_norm-(drone_r+obst_s);
				dgrad[0] = (vo[0]/vo_norm);
				dgrad[1] = (vo[1]/vo_norm);
				dgrad[2] = (vo[2]/vo_norm);

				if (d<=param_brep)
				{
					gradfrep[0] = -(param_krep*((1/d)-(1/param_brep))*(1/(d*d))*dgrad[0]);
					gradfrep[1] = -(param_krep*((1/d)-(1/param_brep))*(1/(d*d))*dgrad[1]);
					gradfrep[2] = -(param_krep*((1/d)-(1/param_brep))*(1/(d*d))*dgrad[2]);
																							}
				else
				{
					gradfrep[0] = 0;
					gradfrep[1] = 0;
					gradfrep[2] = 0;
																							}

    
				gradf[0] = gradfrep[0]+gradfatt[0];
				gradf[1] = gradfrep[1]+gradfatt[1];
				gradf[2] = gradfrep[2]+gradfatt[2];
				gradf_norm = sqrt(gradf[0]*gradf[0] + gradf[1]*gradf[1] + gradf[2]*gradf[2]);
		
				//Enter code to compute dq from the Gradient Descent method


				dq[0] = -delta_t*param_kdescent*gradf[0];
				dq[1] = -delta_t*param_kdescent*gradf[1];
				dq[2] = -delta_t*param_kdescent*gradf[2];
				dqnorm = gradf_norm;
				if (dqnorm>param_bdescent)
				{
					dq[0] = -param_bdescent*(gradf[0]/gradf_norm);
					dq[1] = -param_bdescent*(gradf[1]/gradf_norm);
					dq[2] = -param_bdescent*(gradf[2]/gradf_norm);
				}
				dGoal->Pos.x = goal_q[0];
				dGoal->Pos.y = goal_q[1];
				dGoal->Pos.z = goal_q[2];
				dGoal->Tz = 0;

				prev_time = time;
			}
		}
        else {
			goal_q[0]=drone_q[0]+dq[0];
			goal_q[1]=drone_q[1]+dq[1];
			goal_q[2]=drone_q[2]+dq[2];
       		//printf("command is Null or bad do NOTHING ever, hover\n");
	}
	

		dGoal->Pos.x = goal_q[0];
		dGoal->Pos.y = goal_q[1];
		dGoal->Pos.z = goal_q[2];
		dGoal->Tz = 0;

	}
	
	/*-------- End Simple Desired Position Set ----------------*/
}
/* End Potential Field */
/*----------------------------------------------------------------------*/
/*--------------------------- End Functions ----------------------------*/
/*----------------------------------------------------------------------*/