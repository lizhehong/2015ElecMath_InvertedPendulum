#include "FreePole.h"

POLEtypedef FreePole;

void FreePoleParamsInit(){
		FreePole.AcutalAngle = 0;
		FreePole.AngleMaxpoint = 0;
		FreePole.AngleSpeed = 0;
		FreePole.lastAngle = 0;
		FreePole.Len = 0;//固定值 不是0
		FreePole.LineSpeed = 0;
		FreePole.SetpointAngle = 0;
		FreePole.WorkDistance = 0;

}
