#include "bsp_405x.h"
//test

const float Electronic_dw[3] = {1, 4.48, 45};//设置每个挡位的放大倍数,从左到右1->3挡位

struct _Es_Range
{
	/* data */
	float Range_min;		//挡位最小值
	float Range_max;		//挡位最大值
};

xdata struct _Es_Range Es_Range[3] ={ 
{1, 5},				  //设置第一个挡位的范围
{0.1, 1},			  //设置第二个挡位的范围
{0, 0.1}};			   //设置第三个挡位的范围

static uint8_t  Electronic_Auto_Mode(float val,uint8_t dw);//自动获取合适挡位

static double ES_Val[2] = 0 ;//获取的电压值

double Get_ES_Val(void)	//外部调用获取电压值
{
	return(*ES_Val);
	//return(*(ES_Val + 1));
}

static double Get_ES_Val2(void)	//外部调用获取电压值
{
	return(*(ES_Val + 1));
}

static void Set_ES_Val1(double temp)	//内部调用设置校零电压值
{
	*ES_Val = temp;
}

static void Set_ES_Val2(double temp)	//内部调用设置未校零电压值
{
	*(ES_Val + 1) = temp;
}

uint8_t Electronic_scale_scan(uint8_t _scan,float val)
{
	static uint8_t State_Machine = 1;	//状态机
	static uint8_t last_scan = 0;		//记录上一次输入挡位
	static uint8_t last_dw = 1;			//记录上一次放大挡位(初始化默认为1)
	uint8_t temp = 0;
	if(_scan != last_scan){				//当输入的电子秤编号与上一次不同时进行状态机置1
		last_scan = _scan;
		//设置从第一步开始
		State_Machine = 1;
	}



	switch(State_Machine)
	{
		case 1:
			switch(last_scan)							//打开对应输入开关
			{
				case 0:INPUT_ONE_ENALBE 		break;
				case 1:INPUT_TWO_ENALBE 		break;
				case 2:INPUT_THREE_ENALBE 		break;
				case 3:INPUT_FOUR_ENALBE 		break;
				case 4:INPUT_CLEAR_ENALBE 		break;
				default :INPUT_CLEAR_ENALBE	break;
			}
			OP_dw1_ENALBE;
			State_Machine++;
		break;
		case 2:											//进行数据的获取
			temp = Electronic_Auto_Mode(val, last_dw);
			
			if(last_dw != temp){
				last_dw = temp;
				switch(last_dw)
				{
					case 1:OP_dw1_ENALBE  		break;
					case 2:OP_dw2_ENALBE  		break;
					case 3:OP_dw3_ENALBE  		break;
					default:break;
				}
				State_Machine = 5;
			}
			else{
				Set_ES_Val2(val / Electronic_dw[last_dw-1]);
				State_Machine++;
				INPUT_CLEAR_ENALBE
			}
			break;
		case 3:State_Machine++; break;
		case 4:						//获取校零数据
			//Set_ES_Val1(0.993554*((Get_ES_Val2() - (val / Electronic_dw[last_dw - 1])))+0.0135488) ;
			switch (last_dw)
			{
				case 1:
				if((Get_ES_Val2() - (val / Electronic_dw[last_dw - 1])) >= 4.971917)
					Set_ES_Val1(((4.95-5)/(4.971917 - 4.978022)*(Get_ES_Val2() - (val / Electronic_dw[last_dw - 1]) - 4.971917))+4.95) ;
				else			
					Set_ES_Val1(0.991612*(Get_ES_Val2() - (val / Electronic_dw[last_dw - 1]))+0.0193707) ;
				break;
				case 2:Set_ES_Val1(1.02098*(Get_ES_Val2() - (val / Electronic_dw[last_dw - 1]))+0.00819215) ;break;
				case 3:Set_ES_Val1(1.02078*(Get_ES_Val2() - (val / Electronic_dw[last_dw - 1]))+0.000493174) ;
					  if(Get_ES_Val() <= 0.001) Set_ES_Val1(0);break;
				default:break;
			}
			switch(last_scan)							//打开对应输入开关
			{
				case 0:INPUT_ONE_ENALBE 		break;
				case 1:INPUT_TWO_ENALBE 		break;
				case 2:INPUT_THREE_ENALBE 		break;
				case 3:INPUT_FOUR_ENALBE 		break;
				case 4:INPUT_CLEAR_ENALBE 		break;
				default :INPUT_CLEAR_ENALBE	break;
			}
			State_Machine = 5;//重新获取数据
			break;
		case 5:State_Machine = 2; break;	
		default: break;
	}
	return 0;
}



static uint8_t  Electronic_Auto_Mode(float val,uint8_t dw)
{
	uint8_t state = 0;
	float temp = val / Electronic_dw[dw - 1];//数组存储从0开始
		if(temp > Es_Range[0].Range_min)
			state = 1;
		else if(temp <= Es_Range[1].Range_max && temp > Es_Range[1].Range_min)
			state = 2;
		else if(temp >= Es_Range[2].Range_min)
			state = 3;
	return state;
}





// static uint8_t  Electronic_Auto_Mode(float val,uint8_t dw)
// {
// 	uint8_t state = 0;
// 	float temp = val / Electronic_dw[dw];
// 	switch(dw)
// 	{
// 		case 1: if(temp > Es_Range[0].Rangle_min)
// 					state = 1;
// 				else if(temp <= Es_Range[1].Rangle_max && temp > Es_Range[1].Rangle_min)
// 					state = 2;
// 				else if(temp > 0)
// 					state = 3;
// 		break;
// 		case 2:
// 				if(temp <= Es_Range[1].Rangle_max && temp > Es_Range[1].Rangle_min)
// 					state = 2;
// 				else if(temp > 0)
// 					state = 3;
// 		break;
// 		case 3:
// 				if(temp <= Es_Range[2].Rangle_max && temp > Es_Range[2].Rangle_min)
// 					state = 3;
// 	}
// 	return state;
// }
