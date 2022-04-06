#include "main.h"

#pragma comment (lib,"eSPDI_DM.lib")

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

