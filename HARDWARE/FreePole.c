#include "FreePole.h"

POLEtypedef FreePole;

void FreePoleParamsInit(){
		FreePole.AcutalPosition = 0;
		FreePole.AngleMaxpoint = 0;
		FreePole.AngleSpeed = 0;
		FreePole.lastPosition = 0;
		FreePole.Len = 0;//固定值 不是0
		FreePole.LineSpeed = 0;
		FreePole.SetpointPosition = 0;
		FreePole.WorkDistance = 0;

}
