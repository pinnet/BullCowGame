// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
//#include "Math/UnrealMathUtility.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordList/HiddenWordList.txt");
    FFileHelper::LoadFileToStringArrayWithPredicate(Isograms, *WordListPath, [](const FString& Word) {
          return Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word); 
    });
    SetUpGame(); 
}

void UBullCowCartridge::SetUpGame(){

    HiddenWord = Isograms[FMath::RandRange(0,Isograms.Num()-1)];
    bGameOver = false;
    Lives = HiddenWord.Len() * 2;
    ClearScreen();
    //PrintLine(TEXT("%s"),*HiddenWord);
    PrintLine(TEXT("I am thinking of one of the %i isograms."),Isograms.Num());
    PrintLine(TEXT("Can you guess what it is ?"));
    PrintLine(TEXT("<Please enter your guess or = for a hint (cost a life)>"));   

} 
void UBullCowCartridge::EndGame(bool win){
    bGameOver = true;
    if (!win) PrintLine(TEXT("The hidden word was %s"),*HiddenWord);
    PrintLine(TEXT("GAME OVER"));
    PrintLine(TEXT("Would you like to play again? <Y/N>"));
}

void UBullCowCartridge::ExitGame(){

    PrintLine(TEXT("Bye..."));
    PrintLine(TEXT("<Enter Y to start a new game>"));
    
}


void UBullCowCartridge::OnInput(const FString& Input) // When the player hits enter
{
    if(Input.Len() == 0) return;

    if (bGameOver){
        
        if(Input.ToLower() == TEXT("n"))
        {
            ExitGame();   
        }
        else if(Input.ToLower() == TEXT("y"))
        {
            SetUpGame();   
        }
        return;
    }

    if(Input == TEXT("=")){
        
      Hint(FMath::RandRange(1,10));
      TakeALife();
      return; 
    }
    if(!IsIsogram(Input)){
       PrintLine(TEXT("%s is not an isogram"), *Input);
       return;
    }
    if(Input == HiddenWord){

        PrintLine(TEXT("You are correct"));
        if (Lives == 1){
            PrintLine(TEXT("Congratulations you only had one life left"));
        }
        else {
            PrintLine(TEXT("Congratulations you had %i lives left"),Lives);
        }
        EndGame(true);
        return;
    }

    if(Input.Len() != HiddenWord.Len()){

        PrintLine(TEXT("The hidden word contains"));
        if(Input.Len() > HiddenWord.Len()){
             PrintLine(TEXT("less than %d letters."), Input.Len());
        }
        else {
            PrintLine(TEXT("more than %d letters."), Input.Len());
        }
        return;
    }
    
    PrintLine(TEXT("Not quite right"));
    FBullCowCount BullCowCount = GetBullCows(Input);
    PrintLine(TEXT("You have %i Bulls & %i Cows"),BullCowCount.Bulls,BullCowCount.Cows);

    if(TakeALife()) PrintLine(TEXT("Try Again..."));

}
TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const{

    TArray<FString> ValidWords;
    for (FString Word : WordList){

       if(Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word))  { 
          ValidWords.Emplace(Word);
        }
    }   
    return ValidWords;
}
bool UBullCowCartridge::IsIsogram(const FString& Word){

    for (int32 Index = 0;Index < Word.Len(); Index++ ) {
        for(int32 Comparison=Index +1;Comparison < Word.Len();Comparison++){                
            if (Word[Index] == Word[Comparison]){
                return false;
            }
        }
    }
    return true;
}
FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const{
    
    FBullCowCount Count;

    for (int32 Index = 0; Index < Guess.Len(); Index++)
    {
        if (Guess[Index] == HiddenWord[Index]){
            Count.Bulls++;
            continue;
        }

        for (int32 HWIndex = 0; HWIndex < HiddenWord.Len(); HWIndex++)
        {
            if(Guess[Index] == HiddenWord[HWIndex])
            {
                Count.Cows++;
                break;
            }
        }
    }
    return Count;
}
bool UBullCowCartridge::TakeALife(){
    -- Lives;
    if(Lives <= 0) {
        EndGame(false);
        return false;
    }
    if (Lives == 1){
        PrintLine(TEXT("You only have one life left"));
    }
    else {
        PrintLine(TEXT("You have %i lives left"),Lives);
    }
    return true;
}
void UBullCowCartridge::Hint(int32 Level) const{

    TCHAR Letter = HiddenWord[FMath::RandRange(0,HiddenWord.Len()-1)];
    
    switch(Level){
        case 1:
            PrintLine(TEXT("[The hidden word is %i letters long]"),HiddenWord.Len());
            break;
        case 2:
            PrintLine(TEXT("[The hidden word is an isogram]"));
            break;
        default:
            PrintLine(TEXT("[The hidden word contains one %c ]"),Letter);
    }
 
}