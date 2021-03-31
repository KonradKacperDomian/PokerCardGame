#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <cdk.h>
/*!
* \brief Program do gry z w pokera
* \details Przeciwnikiem w grze jest algorytm realizowany przez funckje opponentRaise. Realizowane funkcje:licytacja,sprawdzenie,zapis stanu gry do pliku i jej późniejsze wznowienie
* \author Konrad Domian 304148
* \date 20.01.2020
*/

#define numberOfRanks 6
#define numberOfSuits 4
#define numberOfCardsInHand 5
#define blind 10
#define N 255
#define startCash 1000

/*!
* \struct Card
* \brief structura Karty w ciele structury znajduje sie wartosc karty i jej kolor
*/

typedef struct Card
{
    int rank;
    int suit;
} Card;

void displayMenu();
void menuChooser(int selectedMenuItem,CDKSCREEN *screen,WINDOW *window);
void displayGameMenu(int(*playerCash), int(*opponetCash),CDKSCREEN *screen, WINDOW *window);
void gameMenuChooser(int(*playerCash), int(*opponetCash), int selectedMenuItem,CDKSCREEN *screen,WINDOW *window);
void game(int cash, int opponnetCash,CDKSCREEN *screen,WINDOW *window);
void oneTure(int(*cash), int(*opponnetCash),CDKSCREEN *screen);
void waitForPressN();
void generateHand(Card(**ptrToHandArray));
void displayPlayerHand(Card(**myHand), int displayInstructNr1ifNULLNr2ifOne, int *cash, int playerPower, int *opponentCash, int opponnetPower,CDKSCREEN *cdkscreen);
void displayInstruct(CDKLABEL *instruct, const char **mesg, CDKSCREEN *cdkscreen, CDK_PARAMS params);
void displayCardLabel(CDKLABEL **label, CDKSCREEN *cdkscreen, Card **myHand, int positionX, int positionY, int cardNumber);
void displayHeadUp(CDKLABEL *label, CDKSCREEN *cdkscreen, char *temp, int positionX, int positionY);
void clenerForDisplayHand(CDKLABEL *label1, CDKLABEL *label2, CDKLABEL *label3, CDKLABEL *label4, CDKLABEL *label5, CDKLABEL *instruct, CDKSCREEN *cdkscreen, CDKLABEL *label6, CDKLABEL *label7, CDKLABEL *label8, CDKLABEL *label9);
int compareCardsByRanks(const void *a, const void *b);
void analyze_hand(Card(**myHand), bool *straight, bool *flush, bool *four, bool *three, int *pairs);
int powerOfPlayerHand(Card(**myHand), bool straight, bool flush, bool four, bool three, int pairs);
int setOpponnetPower();
void cardChanger(Card(**myHand), CDKLABEL *label1, CDKLABEL *label2, CDKLABEL *label3, CDKLABEL *label4, CDKLABEL *label5, CDKSCREEN *cdkscreen);
void functionForCardChanger(Card(**myHand), int cardToChange);
int playerRaise(CDKSCREEN *cdkscreen, CDKLABEL *label8, int *playerCash);
void displayYourRaise(int raiseOfPlayer, CDKSCREEN *cdkscreen, CDKLABEL *label8);
void displayOpponentRaise(int raiseOfopponent, CDKSCREEN *cdkscreen, CDKLABEL *label9);
void saveGame(int(*playerCash), int(*opponetCash));
void loadGame(CDKSCREEN *screen,WINDOW *window);
int opponetRaise(int opponetPower, int *playerCash, int *opponetCash, int playerRaise);
void biding(CDKSCREEN *cdkscreen, CDKLABEL *label8, CDKLABEL *label9, int opponetPower, int *playerCash, int *opponetCash, int playerPower);
void whoWin(int playerPower, int opponetPower, int(*playerCash), int(*opponentCash), int raise, CDKSCREEN *cdkscreen);

/*!
* \fn main
* \brief funkcja sluzaca jako petla
* \details w funkcji inicjowany jest ekran, i wywolywana jest  w niej funkcja displayMenu.
* \return 0
*/

int main()
{
    curs_set(0);
    WINDOW *window = initscr();
    CDKSCREEN *screen = initCDKScreen(window);
    initCDKColor();
    for (;;)
    {
        displayMenu(window,screen);
    }
    destroyCDKScreen(screen);
    endCDK();
}
/*!
* \fn displayMenu
* \brief funkcja sluzaca do wyswietlania menu
* \details w funkcji wyswietlane jest glowne menu, i inicjowana jest funkcja menuChoser
* \param window typ wskaznik do WINDOW
* \param screen typ wskaznik do CDKSCREEN 
*/

void displayMenu(WINDOW *window, CDKSCREEN *screen)
{
    int selectedMenuItem=0;
    const char *menuItems[MAX_MENU_ITEMS][MAX_SUB_ITEMS] = {
        {"Game", "New Game", "Load Game", "Exit"},
        {
            "Info",
            "Help",
            "Author",
        },
    };
    int submenuSizes[2] = {4, 3}, submenuPositions[2] = {LEFT, RIGHT};
    char *text[2] = {"                                          ", "                                          "};
    char tempBuffer[255];
    CDKMENU *menu = newCDKMenu(screen, menuItems, 2, submenuSizes, submenuPositions, TOP, A_UNDERLINE, A_REVERSE);
    refreshCDKScreen(screen);
    selectedMenuItem = activateCDKMenu(menu, 0);
    if (menu->exitType == vNORMAL)
    {
        sprintf(tempBuffer, "Selected -> %s", menuItems[(selectedMenuItem / 100)][(selectedMenuItem % 100) + 1]);
        text[0] = tempBuffer;
        text[1] = "Press any key.";
        popupLabel(screen, (CDK_CSTRING2)text, 2);
    }
    destroyCDKMenu(menu);
    menuChooser(selectedMenuItem, screen,window);
    
}
/*!
* \fn menuChooser
* \brief funkcja sluzaca jako funkcja ktora przypisuje odpowiedniemu przyciskowi w menu swoja funkcjonalnosc.
* \details jezeli do funkcji zostala wyslana wartosc 0 inicjowana jest funkcja game, jezeli 1 to funkcja loadGame a jezeli 2 to zakanczany jest program.
* \param window typ wskaznik do WINDOW
* \param screen typ wskaznik do CDKSCREEN
* \param selectedMenuItem liczba całkowita
*/

void menuChooser(int selectedMenuItem,CDKSCREEN *screen,WINDOW *window)
{
    if (selectedMenuItem == 0)
        game(startCash, startCash,screen,window);
    if (selectedMenuItem == 1)
        loadGame(screen,window);
    if (selectedMenuItem == 2)
        exit(0);
}
/*!
* \fn displayGameMenu
* \brief funkcja sluzaca do wyswietlania menu w grze
* \details w funkcji wyswietlane jest menu gry. i inicjowana jest funkcja gameMenuChooser
* \param window typ wskaznik do WINDOW
* \param screen typ wskaznik do CDKSCREEN
* \param playerCash wskaznik do liczby całkowitej
* \param opponetCash wskaznik do liczby całkowitej
*/

void displayGameMenu(int(*playerCash), int(*opponetCash),CDKSCREEN *screen, WINDOW *window)
{
    int selectedMenuItem=0;
    const char *menuItems[MAX_MENU_ITEMS][MAX_SUB_ITEMS] = {
        {"Pause", "Next Hand", "Load Game", "Save Game", "Back To Main Menu"}};
    int submenuSizes[1] = {5}, submenuPositions[1] = {RIGHT};
    char *text[2] = {"                                          ", "                                          "};
    char tempBuffer[255]={0};
    CDKMENU *menu = newCDKMenu(screen, menuItems, 1, submenuSizes, submenuPositions, TOP, A_UNDERLINE, A_REVERSE);
    refreshCDKScreen(screen);
    selectedMenuItem = activateCDKMenu(menu, 0);
    if (menu->exitType == vNORMAL)
    {
        sprintf(tempBuffer, "Selected -> %s", menuItems[(selectedMenuItem / 100)][(selectedMenuItem % 100) + 1]);
        text[0] = tempBuffer;
        text[1] = "Press any key.";
        popupLabel(screen, (CDK_CSTRING2)text, 2);
    }
    destroyCDKMenu(menu);
    gameMenuChooser(playerCash, opponetCash, selectedMenuItem,screen,window);
}

/*!
* \fn gameMenuChooser
* \brief funkcja sluzaca jako funkcja ktora przypisuje odpowiedniemu przyciskowi w menu gry swoja funkcjonalnosc.
* \details jezeli do funkcji zostala wyslana wartosc 0 inicjowana jest funkcja game, jezeli 1 to funkcja loadGame a jezeli 2 to funkcja saveGame, a jezeli 3 to funkcja displayMenu.
* \param window typ wskaznik do WINDOW
* \param screen typ wskaznik do CDKSCREEN
* \param selectedMenuItem liczba całkowita
* \param playerCash wskaznik do liczby całkowitej
* \param opponetCash wskaznik do liczby całkowitej
*/

void gameMenuChooser(int(*playerCash), int(*opponetCash), int selectedMenuItem,CDKSCREEN *screen,WINDOW *window)
{
    if (selectedMenuItem == 0)
        game(*playerCash,*opponetCash,screen,window);
    if (selectedMenuItem == 1)
        loadGame(screen,window);
    if (selectedMenuItem == 2)
        saveGame(playerCash, opponetCash);
    if (selectedMenuItem == 3)
        main();
}

/*!
* \fn game
* \brief funkcja sluzy do prowadzenia rozgrywki i kontroluje poziom pieniedzy gracza jak i przeciwnika
* \details w funkcji inicjowany jest wskaznik do kasy gracza i przeciwnika nastepnie w petli for inicjowana jest funkcja oneTure nastepnie dispalyGameMenu jezeli pieniadze gracza badz przeciwnika sa rowne badz mniejsze niz zero inicjowana jest funkcja main.
* \param window typ wskaznik do WINDOW
* \param screen typ wskaznik do CDKSCREEN
* \param cash liczba całkowita
* \param opponetCash liczba całkowita
*/

void game(int cash, int opponnetCash,CDKSCREEN *screen, WINDOW *window)
{
    int *ptrToCash = malloc(sizeof(int));
    int *ptrToOpponnetCash = malloc(sizeof(int));
    ptrToCash = &cash;
    ptrToOpponnetCash = &opponnetCash;
    for (;;)
    {
        oneTure(ptrToCash, ptrToOpponnetCash,screen);
        displayGameMenu(ptrToCash, ptrToOpponnetCash,screen,window);
        if (cash < 0)
        {
            main();
        }
        if (opponnetCash < 0)
        {
            main();
        }
    }
}

/*!
* \fn oneTure
* \brief funkcja sluzaca do przeprowadzenia jednego rozdania kart.
* \details w funkcji losowane sa karty, nastepnie w petli for inicjowane sa funkcje analyze_hand powerOfPlayerHand powerOfOpponentHand i displayPlayerHand.
* \param screen typ wskaznik do CDKSCREEN
* \param cash wskaznik do liczby całkowitej
* \param opponnetCash wskaznik do liczby całkowitej
*/

void oneTure(int(*cash), int(*opponnetCash),CDKSCREEN *screen)
{
    int playerPower = 0;
    int opponnetPower = 0;
    int raise = 0;
    *cash -= blind;
    *opponnetCash -= blind;
    bool straight, flush, four, three;
    int pairs;
    Card myHand[2 * numberOfCardsInHand];
    Card *ptrToHandarray[2 * numberOfCardsInHand];
    for (int i = 0; i < (2 * numberOfCardsInHand); i++)
    {
        ptrToHandarray[i] = malloc(sizeof(Card));
        ptrToHandarray[i] = &myHand[i];
    }
    generateHand(ptrToHandarray);
    for (int i = 0; i < 2; i++)
    {
        analyze_hand(ptrToHandarray, &straight, &flush, &four, &three, &pairs);
        playerPower = powerOfPlayerHand(ptrToHandarray, straight, flush, four, three, pairs);
        opponnetPower = setOpponnetPower();
        displayPlayerHand(ptrToHandarray, 0, cash, playerPower, opponnetCash, opponnetPower,screen);
        displayPlayerHand(ptrToHandarray, 2, cash, playerPower, opponnetCash, opponnetPower,screen);
        analyze_hand(ptrToHandarray, &straight, &flush, &four, &three, &pairs);
        powerOfPlayerHand(ptrToHandarray, straight, flush, four, three, pairs);
        if (i == 0)
            displayPlayerHand(ptrToHandarray, 1, cash, playerPower, opponnetCash, opponnetPower,screen);
    }
}

/*!
* \fn waitForPressQ
* \brief funkcja wczytuje wszystkie wpisywane znaki przez gracza, i jezeli wcisnie gracz przycisk q to koczy sie funkcja.
* \param key zmienna typu char
*/

void waitForPressQ()
{
    char key;
    while ((key = (char)getch()) != 'q')
    {
        continue;
    }
}

/*!
* \fn generateHand
* \brief funkcja sluzaca do losowania kart gracza.
* \details w funkcji przypisywana jest wartosc do struktury Card dla rank i dla suit a nastepnie jest sprawdzane czy w tablicy Hand nie ma juz takiej karty jezeli jest rusza ponowne losowanie karty.
* \param ptrToHandarray tablica wkaznikow do structury Card
*/

void generateHand(Card(**ptrToHandArray))
{
    srand(time(NULL));
    for (int i = 0; i < 2 * numberOfCardsInHand; i++)
    {
        ptrToHandArray[i]->rank = rand() % numberOfRanks;
        ptrToHandArray[i]->suit = rand() % numberOfSuits;
        for (int j = 0; j < i; j++)
        {
            if (ptrToHandArray[i]->rank == ptrToHandArray[j]->rank && ptrToHandArray[i]->suit == ptrToHandArray[j]->suit)
                i--;
        }
    }
    qsort(ptrToHandArray, numberOfCardsInHand, sizeof(Card), compareCardsByRanks);
}
/*!
* \fn displayPlayerHand
* \brief funkcja sluzaca do wyswietlania reki gracza
* \details w funkcji wyswietlana jest reka gracza za pomoca funkcji displayCardLabel kart gracza i polecen z tym zwiazanym jezeli parametr displayInstructNr1IfNULLNr2ifOne ma wartosc 1 wyswietlana jest tablica mesg i inicjowana funkcja waitForPressQ  jezeli ma wartosc 2 to jest wyswietlana tablica mesg i inicjowana funkcja CardChanger a jezeli 3 to wyswietlana jest tablica mesg i inicjowana funkcja biding.
* \param cdkscreen typ wskaznik do CDKSCREEN
* \param cash wskaznik do liczby całkowitej
* \param opponnetCash wskaznik do liczby całkowitej
* \param playerPower liczba całkowita
* \param opponnetPower liczba całkowita
* \param displayInstructNr1ifNULLNr2ifOne liczba całkowita
* \param myHand wskaznik do tablicy wkaznikow do structury Card
*/

void displayPlayerHand(Card(**myHand), int displayInstructNr1ifNULLNr2ifOne, int *cash, int playerPower, int *opponentCash, int opponnetPower,CDKSCREEN *cdkscreen)
{
    CDKLABEL *label1, *label2, *label3, *label4, *label5, *label6, *label7, *label8, *label9, *instruct;
    CDK_PARAMS params;
    char arrayOfPokerFigures[9][11] = {"CARD", "PAIR", "TWO PAIRS", "THREE", "STRAIGHT", "FLUSH", "FLUSH", "FOUR", "POKER"};
    const char *mesg[1];
    CDKparseParams(0, 0, &params, CDK_MIN_PARAMS);
    cdkscreen = initCDKScreen(NULL);
    initCDKColor();
    displayCardLabel(&label1, cdkscreen, myHand, 32, 10, 0);
    displayCardLabel(&label2, cdkscreen, myHand, 20, 13, 1);
    displayCardLabel(&label3, cdkscreen, myHand, 44, 13, 2);
    displayCardLabel(&label4, cdkscreen, myHand, 56, 16, 3);
    displayCardLabel(&label5, cdkscreen, myHand, 8, 16, 4);
    char temp[255];
    temp[0] = 0;
    sprintf(temp, "Your Have a %s", arrayOfPokerFigures[playerPower]);
    displayHeadUp(label6, cdkscreen, temp, 23, 1);
    char cashTemp[255];
    sprintf(cashTemp, "Your Acount %i $", *cash);
    displayHeadUp(label7, cdkscreen, cashTemp, 1, 1);
    refreshCDKScreen(cdkscreen);
    if (displayInstructNr1ifNULLNr2ifOne == 1)
    {
        mesg[0] = "</B>1<!B> - change </U>Card1<!U>, </B>2<!B> - change </U>Card2<!U>, </B>3<!B> - change </U>Card3<!U>, </B>4<!B> - change </U>Card4<!U>,</B>5<!B> - change </U>Card5<!U>, </B>q<!B> - </U>next<!U>";
        displayInstruct(instruct, mesg, cdkscreen, params);
        cardChanger(myHand, label1, label2, label3, label4, label5, cdkscreen);
    }
    else
    {
        if (displayInstructNr1ifNULLNr2ifOne == 0)
        {
            mesg[0] = "</B>q<!B> - </U>next<!U>";
            displayInstruct(instruct, mesg, cdkscreen, params);
            waitForPressQ();
        }
        else
        {
            mesg[0] = "</B>1<!B> - Raise </U>100$<!U>, </B>2<!B> - Raise </U>50$<!U>, </B>3<!B> - Raise </U>10$<!U>, </B>4<!B> - Raise </U>5$<!U>,</B>5<!B> - Raise </U>1$<!U>, </B>q<!B> - </U>next<!U>";
            displayInstruct(instruct, mesg, cdkscreen, params);
            biding(cdkscreen, label8, label9, opponnetPower, cash, &opponnetPower, playerPower);
        }
        refreshCDKScreen(cdkscreen);
    }
    clenerForDisplayHand(label1, label2, label3, label4, label5, instruct, cdkscreen, label6, label7, label8, label9);
    destroyCDKLabel(instruct);
}
/*!
* \fn displayInstruct
* \brief funkcja wyswietlna instrukcje
* \param instruct wskaznik do zmiennej typu CKDLABEL
* \param mesg wskaznik do wskaznika typu char
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
* \param params zmiena typu CDK_PARAMS
*/
void displayInstruct(CDKLABEL *instruct, const char **mesg, CDKSCREEN *cdkscreen, CDK_PARAMS params)
{
    instruct = newCDKLabel(cdkscreen,
                           CDKparamValue(&params, 'X', CENTER),
                           CDKparamValue(&params, 'Y', BOTTOM),
                           (CDK_CSTRING2)mesg, 1,
                           CDKparamValue(&params, 'N', FALSE),
                           CDKparamValue(&params, 'S', FALSE));
    refreshCDKScreen(cdkscreen);
}
/*!
* \fn displayInstruct
* \brief funkcja wyswietlna jedna Karte
* \param label wskaznik do wskaznika do zmiennej typu CKDLABEL
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
* \param myHand wskażnik do tablicy wkaznikow do structury Card
* \param positionX liczba całkowita
* \param positionY liczba całkowita
* \param cardNumber liczba całkowita
*/

void displayCardLabel(CDKLABEL **label, CDKSCREEN *cdkscreen, Card **myHand, int positionX, int positionY, int cardNumber)
{
    char temp[255] = {'\0'};
    const char *mesg1[10];
    char arrayOfRanks[numberOfRanks][7] = {"NINE ", "TEEN ", "JACK ", "QUEEN ", "KING ", "ACE "};
    char arrayOfSuits[numberOfSuits][9] = {"Clubs", "Diamonds", "Hearts", "Spades"};
    strcat(temp, arrayOfRanks[myHand[cardNumber]->rank]);
    strcat(temp, arrayOfSuits[myHand[cardNumber]->suit]);
    mesg1[0] = temp;
    mesg1[1] = "                    ";
    mesg1[2] = "                    ";
    mesg1[3] = "                    ";
    mesg1[4] = "                    ";
    mesg1[5] = "                    ";
    mesg1[6] = "                    ";
    mesg1[7] = "                    ";
    mesg1[8] = "                    ";
    mesg1[9] = "                    ";
    *label = newCDKLabel(cdkscreen, positionX, positionY, (CDK_CSTRING2)mesg1, 10, TRUE, FALSE);
    refreshCDKScreen(cdkscreen);
}
/*!
* \fn displayHeadUp
* \brief funkcja wyswietlna naglowek
* \param label wskaznik do zmiennej typu CKDLABEL
* \param temp wskaznik do zmiennej typu char
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
* \param positionX liczba całkowita
* \param positionY liczba całkowita
*/
void displayHeadUp(CDKLABEL *label, CDKSCREEN *cdkscreen, char *temp, int positionX, int positionY)
{
    const char *mesg6[3];
    mesg6[0] = "                    ";
    mesg6[1] = temp;
    mesg6[2] = "                    ";
    label = newCDKLabel(cdkscreen, positionX, positionY, (CDK_CSTRING2)mesg6, 3, TRUE, FALSE);
}

/*!
* \fn clenerForDisplayHand
* \brief funkcja sluzy do czyszczenia ekranu ze wszystkich naklejek i instrukcji
* \param label1 wskaznik do zmiennej typu CKDLABEL
* \param label2 wskaznik do zmiennej typu CKDLABEL
* \param label3 wskaznik do zmiennej typu CKDLABEL
* \param label4 wskaznik do zmiennej typu CKDLABEL
* \param label5 wskaznik do zmiennej typu CKDLABEL
* \param label6 wskaznik do zmiennej typu CKDLABEL
* \param label7 wskaznik do zmiennej typu CKDLABEL
* \param label8 wskaznik do zmiennej typu CKDLABEL
* \param label9 wskaznik do zmiennej typu CKDLABEL
* \param instruct wskaznik do zmiennej typu CKDLABEL
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
*/

void clenerForDisplayHand(CDKLABEL *label1, CDKLABEL *label2, CDKLABEL *label3, CDKLABEL *label4, CDKLABEL *label5, CDKLABEL *instruct, CDKSCREEN *cdkscreen, CDKLABEL *label6, CDKLABEL *label7, CDKLABEL *label8, CDKLABEL *label9)
{
    destroyCDKLabel(label1);
    destroyCDKLabel(label2);
    destroyCDKLabel(label3);
    destroyCDKLabel(label4);
    destroyCDKLabel(label5);
    destroyCDKLabel(label6);
    destroyCDKLabel(label7);
    destroyCDKLabel(label8);
    destroyCDKLabel(label9);
    destroyCDKLabel(instruct);
    destroyCDKScreen(cdkscreen);
    endCDK();
}
/*!
* \fn clenerForDisplayHand
* \brief funkcja porównuje karty odwzgledem parametru rank jezeli karta pierwsza jest wieksza niz druga zwracana jest wartosc 1 jezeli mniejsza -1 a jezeli sa takie same to 0
* \param a wskaznik do zmiennej typu void
* \param a wskaznik do zmiennej typu void
* \return -1
* \return 0
* \return 1
*/

int compareCardsByRanks(const void *a, const void *b)
{
    const Card *firstCard = a;
    const Card *secondCard = b;
    if (firstCard->rank < secondCard->rank)
        return -1;
    else if (firstCard->rank == secondCard->rank)
        return 0;
    else
        return 1;
}
/*!
* \fn analyze_hand
* \brief funkcja sluzy do analizowania funkcji pod wzgledem strita, koloru, czworki, trojki i par
* \param straight wskaznik do wyrazenie typu bool
* \param flush wskaznik do wyrazenie typu bool
* \param four wskaznik do wyrazenie typu bool
* \param three wskaznik do wyrazenie typu bool
* \param pairs wskaznik do liczba całkowita
* \param myHand wskaznik do tablica wkaznikow do structury Card
*/

void analyze_hand(Card(**myHand), bool *straight, bool *flush, bool *four, bool *three, int *pairs)
{
    //strit
    int i=0;
    while (myHand[i]->rank==(myHand[1+i]->rank-1))
    i++;
    if(i==5)
    *straight=true;
    //kolor
    i=0;
    while (myHand[i]->suit==(myHand[1+i]->suit))
    i++;
    if(i==5) *flush=true;
    //pary
    i=0;
    unsigned int numberOfSameCards=0;
    for(int j=0;j<numberOfCardsInHand;j++)
    {
    while (myHand[i]->rank==myHand[i+1]->rank) 
    {
        numberOfSameCards++;
        i++;
    }
    if(numberOfSameCards==2)
        (*pairs)++;
    if(numberOfSameCards==3)
        *three=true;
    if(numberOfSameCards==4)
    {
        *four=true;
        break;
    }
    }
}

/*!
* \fn powerOfPlayerHand
* \brief funkcja sluzy do przeksztalcenia wyrazen boolowskich straight flush four three i wyrazenia typu int pairs na sile reki gracza wyrazona zmienna typu int
* \param straight wyrazenie typu bool
* \param flush wyrazenie typu bool
* \param four wyrazenie typu bool
* \param three wyrazenie typu bool
* \param pairs liczba całkowita
* \param myHand wskaznik do tablicy wkaznikow do structury Card
* \return power
*/

int powerOfPlayerHand(Card(**myHand), bool straight, bool flush, bool four, bool three, int pairs)
{
    int power;
    if (straight && flush)
        power = 8;
    else if (four)
        power = 7;
    else if (three == 1 && pairs == 1)
        power = 6;
    else if (flush)
        power = 5;
    else if (straight)
        power = 4;
    else if (three)
        power = 3;
    else if (pairs == 2)
        power = 2;
    else if (pairs == 1)
        power = 1;
    else
        power = 0;
    return power;
}

/*!
* \fn setOpponnetPower
* \brief funkcja sluzy do wylosowanie mocy reki przeciwnika zgodnie z pewnym prawdobodobienstwem
* \return 0;
* \return 1;
* \return 2;
* \return 3;
* \return 4;
* \return 5;
* \return 6;
* \return 7;
* \return 8;
*/

int setOpponnetPower()
{
    int power;
    srand(time(NULL));
    power = rand() % 101;
    if (power <= 5)
        return 0;
    else if (power > 5 && power <= 25)
        return 1;
    else if (power > 25 && power <= 40)
        return 2;
    else if (power > 40 && power <= 55)
        return 3;
    else if (power > 550 && power <= 70)
        return 4;
    else if (power > 70 && power <= 85)
        return 5;
    else if (power > 85 && power <= 95)
        return 6;
    else if (power > 95 && power <= 98)
        return 7;
    else
        return 8;
}
/*!
* \fn cardChanger
* \brief funkcja sluzy do wymiany kart na inne przez gracza
* \details przez to ze wygenerowalismy dwie dlonie to zamieniamy karty z jednej reki do drugiej
* \param label1 wskaznik do zmiennej typu CKDLABEL
* \param label2 wskaznik do zmiennej typu CKDLABEL
* \param label3 wskaznik do zmiennej typu CKDLABEL
* \param label4 wskaznik do zmiennej typu CKDLABEL
* \param label5 wskaznik do zmiennej typu CKDLABEL
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
* \param myHand tablica wkaznikow do structury Card
*/

void cardChanger(Card(**myHand), CDKLABEL *label1, CDKLABEL *label2, CDKLABEL *label3, CDKLABEL *label4, CDKLABEL *label5, CDKSCREEN *cdkscreen)
{
    int if5exit = 1;
    char ch;
    while ((ch = (char)getch()) != 'q' && if5exit != 5)
    {
        if5exit++;
        switch (ch)
        {
        case '1':
            destroyCDKLabel(label5);
            functionForCardChanger(myHand,5);
            refreshCDKScreen(cdkscreen);
            break;
        case '2':
            destroyCDKLabel(label2);
            functionForCardChanger(myHand,2);
            refreshCDKScreen(cdkscreen);
            break;
        case '3':
            destroyCDKLabel(label1);
            functionForCardChanger(myHand,1);
            refreshCDKScreen(cdkscreen);
            break;
            functionForCardChanger(myHand,3);
            refreshCDKScreen(cdkscreen);
            break;
        case '5':
            destroyCDKLabel(label4);
            functionForCardChanger(myHand,1);
            refreshCDKScreen(cdkscreen);
            break;
        default:
            continue;
        }
        refreshCDKScreen(cdkscreen);
    }
    qsort(myHand, numberOfCardsInHand, sizeof(Card), compareCardsByRanks);
}

/*!
* \fn functionForCardChanger
* \brief funkcja sluzy do wymiany kart na inne przez gracza
* \details funkcja zasepuje dwie czesto powtarzajace sie linie w funkcji cardChanger
* \param myHand tablica wkaznikow do structury Card
* \param cardTochange liczba całkowita
*/

void functionForCardChanger(Card(**myHand), int cardToChange)
{
    myHand[cardToChange - 1]->rank = myHand[4 + cardToChange]->rank;
    myHand[cardToChange - 1]->suit = myHand[4 + cardToChange]->suit;
}
/*!
* \fn playerRaise
* \brief funkcja sluzy do licytacji a dokladnie do podbicia stawki przez gracza
* \details przez to funkcje switch mamy ustawione o jakie kwoty mozemy podbijac dopoki gracz nie kliknie przycisku q stawki beda sie sumowac.
* \param playerCash wskaznik do liczby całkowitej
* \param label8 wskaznik do zmiennej typu CKDLABEL
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
* \return raiseOfPlayer
*/

int playerRaise(CDKSCREEN *cdkscreen, CDKLABEL *label8, int *playerCash)
{
    char ch;
    static int numberOfOpen;
    numberOfOpen++;
    static int raiseOfPlayer = 0;
    if(numberOfOpen%2==1)
    raiseOfPlayer=0;
    while ((ch = (char)getch()) != 'q')
    {
        switch (ch)
        {
        case '1':
            raiseOfPlayer += 100;
            displayYourRaise(raiseOfPlayer, cdkscreen, label8);
            break;
        case '2':
            raiseOfPlayer += 50;
            displayYourRaise(raiseOfPlayer, cdkscreen, label8);
            break;
        case '3':
            raiseOfPlayer += 10;
            displayYourRaise(raiseOfPlayer, cdkscreen, label8);
            break;
        case '4':
            raiseOfPlayer += 5;
            displayYourRaise(raiseOfPlayer, cdkscreen, label8);
            break;
        case '5':
            raiseOfPlayer += 1;
            displayYourRaise(raiseOfPlayer, cdkscreen, label8);
            break;
        default:
            continue;
        }
    }
    if (raiseOfPlayer > *playerCash)
    {
        raiseOfPlayer = *playerCash;
        return raiseOfPlayer;
    }
    return raiseOfPlayer;
}

/*!
* \fn displayYourRaise
* \brief funkcja sluzy do wyswietlenia licznika podbicia gracza
* \param raiseOfPlayer liczba całkowita
* \param label8 wskaznik do zmiennej typu CKDLABEL
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
*/

void displayYourRaise(int raiseOfPlayer, CDKSCREEN *cdkscreen, CDKLABEL *label8)
{
    char temp[255];
    const char *mesg8[10];
    temp[0] = 0;
    sprintf(temp, "Your raise Do %i $", raiseOfPlayer);
    mesg8[0] = "                    ";
    mesg8[1] = temp;
    mesg8[2] = "                    ";
    label8 = newCDKLabel(cdkscreen, 46, 1, (CDK_CSTRING2)mesg8, 3, TRUE, FALSE);
    refreshCDKScreen(cdkscreen);
}

/*!
* \fn displayOpponentRaise
* \brief funkcja sluzy do wyswietlenia licznika podbicia gracza
* \param raiseOfopponnet liczba całkowita
* \param label9 wskaznik do zmiennej typu CKDLABEL
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
*/

void displayOpponentRaise(int raiseOfopponent, CDKSCREEN *cdkscreen, CDKLABEL *label9)
{
    char temp[255];
    const char *mesg9[10];
    temp[0] = 0;
    sprintf(temp, "Opponent raise Do %i $", raiseOfopponent);
    mesg9[0] = "                    ";
    mesg9[1] = temp;
    mesg9[2] = "                    ";
    label9 = newCDKLabel(cdkscreen, 69, 1, (CDK_CSTRING2)mesg9, 3, TRUE, FALSE);
    refreshCDKScreen(cdkscreen);
}

/*!
* \fn saveGame
* \brief funkcja sluzy do zapisu stanu gry
* \details plik zapisu gry nazywa sie save i jest tworzony w folderu z plikiem wykonywalnym gry
* \param playerCash wskaznik do liczby całkowitej
* \param opponetCash wskaznik do liczby całkowitej
*/


void saveGame(int(*playerCash), int(*opponetCash))
{
    FILE *fp;
    if ((fp = fopen("save", "w")) == NULL)
    {
        printf("Error\n'cannnot Save a Game'");
    }
    else
    {
        fprintf(fp, "Player Cash %d Opponnet Cash %d", (*playerCash), (*opponetCash));
    }
    fclose(fp);
}

/*!
* \fn loadGame
* \brief funkcja sluzy do wczytania stanu gry
* \details funkcja szuka w folderze w ktorym znajduje sie plik wykonywalny gry pliku o nazwie save jezeli nie znajdzie takiego pliku gra wylacza sie a w terminalu wyswietlona zostaje wiadomosc o bledzie
* \param screen wskaznik do zmiennej typu CDKSCREEN
* \param window typ wskaznik do WINDOW
*/

void loadGame(CDKSCREEN *screen,WINDOW *window)
{
    int playerCash;
    int opponnetCash;
    char arrayOfCash[N], trash[N];
    int *ptrToCash = malloc(sizeof(int));
    int *ptrToOpponnetCash = malloc(sizeof(int));
    FILE *fp;
    if ((fp = fopen("save", "r")) == NULL)
    {
        printf("Error\ncannot Load a Game");
        exit(0);
    }
    else
    {
        fgets(arrayOfCash, N, fp);
    }
    sscanf(arrayOfCash, "%s %s %d %s %s %d", trash, trash, &playerCash, trash, trash, &opponnetCash);
    ptrToCash = &playerCash;
    ptrToOpponnetCash = &opponnetCash;
    game(*ptrToCash, *ptrToOpponnetCash,screen,window);
}

/*!
* \fn opponetRaise
* \brief algorytm podbicia przez przeciwnika
* \details przez specjalny wzór który uwzglednia pieniadze gracza i przeciwnika, przebicie gracza jak i wage kart przeciwnika obliczane jest przebicie przeciwnika.
* \param playerCash wskaznik do liczby całkowitej
* \param opponnetCash wskaznik do liczby całkowitej
* \param playerPower liczba całkowita
* \param opponnetPower liczba całkowita
* \return opponetRaise
*/

int opponetRaise(int opponetPower, int *playerCash, int *opponetCash, int playerRaise)
{
    srand(time(NULL));
    int opponetRaise = (((rand() % 13) + opponetPower) / ((rand() % 6) + 1)) * playerRaise;
    if (opponetRaise > *opponetCash || opponetPower > 7 || opponetRaise > *playerCash)
        opponetRaise = playerRaise;
    return opponetRaise;
}

/*!
* \fn biding
* \brief funkcja sluzy do prowadzenia licytacji
* \param playerCash wskaznik do liczby całkowitej
* \param opponnetCash wskaznik do liczby całkowitej
* \param playerPower liczba całkowita
* \param opponnetPower liczba całkowita
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
* \param label8 wskaznik do zmiennej typu CKDLABEL
* \param label9 wskaznik do zmiennej typu CKDLABEL
*/

void biding(CDKSCREEN *cdkscreen, CDKLABEL *label8, CDKLABEL *label9, int opponetPower, int *playerCash, int *opponetCash, int playerPower)
{
    static int numberOfOpen;
    numberOfOpen = 0;
    numberOfOpen++;
    int pRaise = playerRaise(cdkscreen, label8, playerCash);
    int oRaise = opponetRaise(opponetPower, playerCash, opponetCash, pRaise);
    displayOpponentRaise(oRaise, cdkscreen, label9);
    waitForPressQ();
    if (pRaise == 0)
    {
        whoWin(8, 0, playerCash, opponetCash, 0, cdkscreen);
        return;
    }
    if (pRaise == 0)
    {
        whoWin(0, 8, playerCash, opponetCash, 0, cdkscreen);
        return;
    }
    while (pRaise != oRaise)
    {
        pRaise = playerRaise(cdkscreen, label8, playerCash);
        oRaise = opponetRaise(opponetPower, playerCash, opponetCash, pRaise);
        displayOpponentRaise(oRaise, cdkscreen,label9);
        waitForPressQ();
        int randNum;
        srand(time(NULL));
        randNum = rand() % 6;
        if (randNum == 5)
        {
            break;
        }
    }
    if (numberOfOpen % 2 == 0)
    {
        whoWin(playerPower, opponetPower, playerCash, opponetCash, pRaise, cdkscreen);
        *playerCash -= pRaise;
        *opponetCash -= pRaise;
    }
}
/*!
* \fn whoWin
* \brief funkcja sluzy do sprawdzenia kto wygrał dane rozdanie kart
* \param playerPower liczba całkowita
* \param opponetPower liczba całkowita
* \param playerCash wskaznik do liczby całkowitej
* \param opponentCash wskaznik do liczby całkowitej
* \param raise liczba całkowita
* \param cdkscreen wskaznik do zmiennej typu CDKSCREEN
*/

void whoWin(int playerPower, int opponetPower, int(*playerCash), int(*opponentCash), int raise, CDKSCREEN *cdkscreen)
{
    const char *mesg10[3];
    CDKLABEL *label10;
    if (playerPower > opponetPower)
    {
        printf("You win");
        *opponentCash -= (2 * raise + blind);
        *playerCash += (2 * raise + blind);
        mesg10[0] = "                    ";
        mesg10[1] = "      YOU WIN       ";
        mesg10[2] = "press q to next hand";
        label10 = newCDKLabel(cdkscreen, 30, 15, (CDK_CSTRING2)mesg10, 3, TRUE, FALSE);
        refreshCDKScreen(cdkscreen);
    }
    else
    {
        printf("You loose");
        *opponentCash += (2 * raise + blind);
        *playerCash -= (2 * raise + blind);
        mesg10[0] = "                    ";
        mesg10[1] = "      YOU LOOSE     ";
        mesg10[2] = "press q to next hand";
        label10 = newCDKLabel(cdkscreen, 30, 15, (CDK_CSTRING2)mesg10, 3, TRUE, FALSE);
        refreshCDKScreen(cdkscreen);
    }
    waitForPressQ();
}
