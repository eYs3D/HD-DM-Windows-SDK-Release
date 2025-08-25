#include "main.h"

#pragma comment (lib,"eSPDI_DM.lib")
#pragma comment(lib, "setupapi.lib" )

int main(void)
{
    int input = 0;
    bool exitLoop = false;

    do
    {
        printf("List Testing Items:\n");
        printf(" 0. Exit test. \n");
        printf(" 1. Read3X\n");
        printf(" 2. Write3X\n");
        printf(" 3. Read4X\n");
        printf(" 4. Write4X\n");
        printf(" 5. Read5X\n");
        printf(" 6. Write5X\n");
        printf(" 7. Read24X\n");
        printf(" 8. Write24X\n");
        printf(" 9. Reset UNPData\n");
        printf("10. GetUserData\n"); //Issue 6882
        printf("11. Get Color&Depth Image\n");
        printf("12. Get Color&Depth Image - print all\n");
        printf("13. Get Point Cloud\n");
		printf("14. Copy from G1 to G2\n");
		printf("15. Set analog & digital gain for IVY\n");
		printf("16. IMU Callback\n");
		printf("17. IMU New API\n");
		printf("18. Reset Bootloader\n");
		printf("19. Batch ASIC register value\n");
		printf("20. Batch ASIC & Sensor & FW register value\n");
		printf("21. Loop init get image close release)\n");

        printf("\nPlease choose the testing item:\n");
        scanf("%d", &input);

        switch(input)
        {
            case 0 : // Exit test
            {
                exitLoop = true;
                break;
            }

            case 1: //1. Read3X
            {
                Read3X();
                break;
            }

            case 2: //2. Write3X
            {
                Write3X();
                break;
            }

            case 3: //3. Read4X
            {
                Read4X();
                break;
            }

            case 4: //4. Write4X
            {
                Write4X();
                break;
            }

            case 5: //5. Read5X
            {
                Read5X();
                break;
            }

            case 6: //6. Write5X
            {
                Write5X();
                break;
            }

            case 7: //7. Read24X
            {
                Read24X();
                break;
            }

            case 8: //8. Write24X
            {
                Write24X();
                break;
            }
            case 9: //9. ResetUNPData
            {
                ResetUNPData();
                break;
            }
            case 10: //10. GetUserData for Issue 6882
            {
                GetUserData();
                break;
            }
            case 11:
            {
                GetColorDepthImage(false);
                break;
            }
            case 12:
            {
                GetColorDepthImage(true);
                break;
            }
            case 13:
            {
                GetPointCloud();
                break;
            }
			case 14:
			{
				CopyFromG1ToG2();
				break;
			}
			case 15:
			{
				SetAnalogAndDigitalGainExample();
				break;
			}
			case 16:
			{
				IMUCallbackDemo();
				break;
			}
			case 17:
			{
				IMUAPIDemo();
				break;
			}
			case 18:
			{
				ResetBootloader();
				break;
			}
			case 19:
			{
				BatchReadASIC();
				break;
			}
			case 20:
			{
				BatchReadASICSensorFW();
				break;
			}
			case 21:
			{
				InitOpenCloseReleaseLoop();
				break;
			}

            default:
            {
                printf("Error : The wrong input.\n");
                break;
            }

        }

        if (exitLoop == true)
        {
            break;
        }

    } while (true);

    return 0;
}

