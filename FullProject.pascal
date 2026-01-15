program MiniProjectGame;

uses SysUtils;

type
    PlayerPtr = ^Player;
    Player = record
        id: Integer;
        name: String[30];
        age: Integer;
        score: Integer;
        wins: Integer;
        losses: Integer;
        consecutiveWins: Integer;
        consecutiveLosses: Integer;
        next: PlayerPtr;
    end;

    Queue = record
        head: PlayerPtr;
        tail: PlayerPtr;
    end;

var
    F, F1, F3: Queue;
    LG, LP: PlayerPtr;

// ------------------ Queue Operations ------------------
procedure Enqueue(var q: Queue; p: PlayerPtr);
begin
    p^.next := nil;
    if q.tail = nil then
    begin
        q.head := p;
        q.tail := p;
    end
    else
    begin
        q.tail^.next := p;
        q.tail := p;
    end;
end;

function Dequeue(var q: Queue): PlayerPtr;
begin
    if q.head = nil then
        Dequeue := nil
    else
    begin
        Dequeue := q.head;
        q.head := q.head^.next;
        if q.head = nil then q.tail := nil;
        Dequeue^.next := nil;
    end;
end;

// ------------------ Utility Functions ------------------
function GCD(a, b: Integer): Integer;
var temp: Integer;
begin
    while b <> 0 do
    begin
        temp := b;
        b := a mod b;
        a := temp;
    end;
    GCD := a;
end;

function ContainsDigit(num, digit: Integer): Boolean;
begin
    while num > 0 do
    begin
        if (num mod 10) = digit then
        begin
            ContainsDigit := True;
            Exit;
        end;
        num := num div 10;
    end;
    ContainsDigit := False;
end;

function SumDigits(num: Integer): Integer;
var sum: Integer;
begin
    sum := 0;
    while num > 0 do
    begin
        sum := sum + (num mod 10);
        num := num div 10;
    end;
    SumDigits := sum;
end;

function CheckScore(n1, n2: Integer): Boolean;
var g, temp, d: Integer;
begin
    g := GCD(n1, n2);
    temp := g;
    while temp > 0 do
    begin
        d := temp mod 10;
        if ContainsDigit(n1, d) or ContainsDigit(n2, d) then
        begin
            CheckScore := True;
            Exit;
        end;
        temp := temp div 10;
    end;
    CheckScore := False;
end;

// ------------------ LG / LP Insert ------------------
procedure InsertLG(p: PlayerPtr);
var cur: PlayerPtr;
begin
    p^.next := nil;
    if (LG = nil) or (p^.score > LG^.score) then
    begin
        p^.next := LG;
        LG := p;
    end
    else
    begin
        cur := LG;
        while (cur^.next <> nil) and (cur^.next^.score >= p^.score) do
            cur := cur^.next;
        p^.next := cur^.next;
        cur^.next := p;
    end;
end;

procedure InsertLP(p: PlayerPtr);
begin
    p^.next := LP;
    LP := p;
end;

// ------------------ Display State ------------------
procedure DisplayState(F, F1, F3: Queue);
var cur: PlayerPtr;
begin
    WriteLn('--- Current State ---');
    Write('Queue F: ');
    cur := F.head;
    while cur <> nil do
    begin
        Write(cur^.name, ' ');
        cur := cur^.next;
    end;
    WriteLn;
    Write('Queue F1: ');
    cur := F1.head;
    while cur <> nil do
    begin
        Write(cur^.name, ' ');
        cur := cur^.next;
    end;
    WriteLn;
    Write('Queue F3: ');
    cur := F3.head;
    while cur <> nil do
    begin
        Write(cur^.name, ' ');
        cur := cur^.next;
    end;
    WriteLn;
    Write('LG (Winners): ');
    cur := LG;
    while cur <> nil do
    begin
        Write(cur^.name, '(', cur^.score, ') ');
        cur := cur^.next;
    end;
    WriteLn;
    Write('LP (Losers): ');
    cur := LP;
    while cur <> nil do
    begin
        Write(cur^.name, ' ');
        cur := cur^.next;
    end;
    WriteLn;
    WriteLn('---------------------');
end;

// ------------------ Player Selection ------------------
function SelectPlayer(var F, F1, F3: Queue): PlayerPtr;
begin
    if F1.head <> nil then SelectPlayer := Dequeue(F1)
    else if F.head <> nil then SelectPlayer := Dequeue(F)
    else if F3.head <> nil then SelectPlayer := Dequeue(F3)
    else SelectPlayer := nil;
end;

// ------------------ Play Round Part I ------------------
procedure PlayRoundPart1(p1, p2: PlayerPtr; var F, F1, F3: Queue);
var score1, score2, valuesGenerated, val, sum: Integer;
begin
    score1 := 0; score2 := 0; valuesGenerated := 0;

    while (Abs(score1 - score2) < 3) and (valuesGenerated < 12) do
    begin
        val := Random(1000000);
        sum := SumDigits(val);
        if sum mod 5 = 0 then
        begin
            if valuesGenerated mod 2 = 0 then Inc(score1)
            else Inc(score2);
        end;
        Inc(valuesGenerated);
    end;

    WriteLn('Result (Part I): ', p1^.name, '(', score1, ') vs ', p2^.name, '(', score2, ')');

    if score1 > score2 then
    begin
        Inc(p1^.wins); Inc(p1^.consecutiveWins);
        Inc(p2^.losses); Inc(p2^.consecutiveLosses);
        p1^.score := p1^.score + score1;
        p2^.score := p2^.score + score2;

        if p1^.consecutiveWins >= 3 then Enqueue(F1, p1)
        else Enqueue(F, p1);

        if p2^.consecutiveLosses >= 3 then Enqueue(F3, p2)
        else Enqueue(F, p2);

        if p1^.wins >= 5 then InsertLG(p1);
        if p2^.losses >= 5 then InsertLP(p2);
    end
    else if score2 > score1 then
    begin
        Inc(p2^.wins); Inc(p2^.consecutiveWins);
        Inc(p1^.losses); Inc(p1^.consecutiveLosses);
        p1^.score := p1^.score + score1;
        p2^.score := p2^.score + score2;

        if p2^.consecutiveWins >= 3 then Enqueue(F1, p2)
        else Enqueue(F, p2);

        if p1^.consecutiveLosses >= 3 then Enqueue(F3, p1)
        else Enqueue(F, p1);

        if p2^.wins >= 5 then InsertLG(p2);
        if p1^.losses >= 5 then InsertLP(p1);
    end
    else
    begin
        Enqueue(F, p1);
        Enqueue(F, p2);
    end;
end;

// ------------------ Play Round Part II ------------------
procedure PlayRoundPart2(p1, p2: PlayerPtr; var F, F1, F3: Queue);
var score1, score2, valuesGenerated, n1, n2: Integer;
begin
    score1 := 0; score2 := 0; valuesGenerated := 0;

    while (Abs(score1 - score2) < 3) and (valuesGenerated < 16) do
    begin
        n1 := Random(1000) + 1;
        n2 := Random(1000) + 1;
        if CheckScore(n1, n2) then
        begin
            if valuesGenerated mod 2 = 0 then Inc(score1)
            else Inc(score2);
        end;
        Inc(valuesGenerated);
    end;

    WriteLn('Result (Part II): ', p1^.name, '(', score1, ') vs ', p2^.name, '(', score2, ')');

    if score1 > score2 then
    begin
        Inc(p1^.wins); Inc(p1^.consecutiveWins);
        Inc(p2^.losses); Inc(p2^.consecutiveLosses);
        p1^.score := p1^.score + score1;
        p2^.score := p2^.score + score2;

        if p1^.consecutiveWins >= 2 then InsertLG(p1)
        else Enqueue(F1, p1);

                if p2^.losses >= 2 then InsertLP(p2)
        else Enqueue(F3, p2);
    end
    else if score2 > score1 then
    begin
        Inc(p2^.wins); Inc(p2^.consecutiveWins);
        Inc(p1^.losses); Inc(p1^.consecutiveLosses);
        p1^.score := p1^.score + score1;
        p2^.score := p2^.score + score2;

        if p2^.consecutiveWins >= 2 then InsertLG(p2)
        else Enqueue(F1, p2);

        if p1^.losses >= 2 then InsertLP(p1)
        else Enqueue(F3, p1);
    end
    else
    begin
        Enqueue(F, p1);
        Enqueue(F, p2);
    end;
end;

// ------------------ Print Top 3 Winners ------------------
procedure PrintTopWinners;
var cur: PlayerPtr; count: Integer;
begin
    WriteLn('=== TOP 3 WINNERS ===');
    cur := LG; count := 0;
    while (cur <> nil) and (count < 3) do
    begin
        WriteLn(count+1, '. ', cur^.name, ' with ', cur^.score, ' points');
        cur := cur^.next;
        Inc(count);
    end;
    if count = 0 then WriteLn('No winners recorded.');
end;

// ------------------ Main ------------------
var
    numPlayers, i, round, maxRoundsPart1, maxRoundsPart2: Integer;
    p1, p2, p: PlayerPtr;
begin
    Randomize;
    F.head := nil; F.tail := nil;
    F1.head := nil; F1.tail := nil;
    F3.head := nil; F3.tail := nil;
    LG := nil; LP := nil;

    Write('Enter number of players (>=2): ');
    ReadLn(numPlayers);
    while numPlayers < 2 do
    begin
        WriteLn('At least 2 players are required.');
        Write('Enter number of players (>=2): ');
        ReadLn(numPlayers);
    end;

    for i := 1 to numPlayers do
    begin
        New(p);
        p^.id := i;
        Write('Enter name for Player ', i, ': ');
        ReadLn(p^.name);
        Write('Enter age: ');
        ReadLn(p^.age);
        p^.score := 0; p^.wins := 0; p^.losses := 0;
        p^.consecutiveWins := 0; p^.consecutiveLosses := 0;
        p^.next := nil;
        Enqueue(F, p);
    end;

    round := 1;
    maxRoundsPart1 := 3 * numPlayers;
    maxRoundsPart2 := 2 * numPlayers;

    // ----------- Part I Strategy -----------
    while (round <= maxRoundsPart1) and ((F.head <> nil) or (F1.head <> nil) or (F3.head <> nil)) do
    begin
        p1 := SelectPlayer(F, F1, F3);
        p2 := SelectPlayer(F, F1, F3);
        if (p1 = nil) or (p2 = nil) then break;

        WriteLn('--- Part I Round ', round, ' ---');
        PlayRoundPart1(p1, p2, F, F1, F3);
        DisplayState(F, F1, F3);
        Inc(round);
    end;

    // ----------- Part II Strategy -----------
    round := 1;
    while (round <= maxRoundsPart2) and ((F.head <> nil) or (F1.head <> nil) or (F3.head <> nil)) do
    begin
        p1 := SelectPlayer(F, F1, F3);
        p2 := SelectPlayer(F, F1, F3);
        if (p1 = nil) or (p2 = nil) then break;

        WriteLn('--- Part II Round ', round, ' ---');
        PlayRoundPart2(p1, p2, F, F1, F3);
        DisplayState(F, F1, F3);
        Inc(round);
    end;

    // ----------- End of Game Rules (Part II forced placement) -----------
    if (F.head <> nil) or (F1.head <> nil) or (F3.head <> nil) then
    begin
        WriteLn('--- Forced End of Game Placement ---');
        while F1.head <> nil do
        begin
            p := Dequeue(F1);
            InsertLG(p);   // F1 → LG
        end;
        while F.head <> nil do
        begin
            p := Dequeue(F);
            InsertLP(p);   // F → LP
        end;
        while F3.head <> nil do
        begin
            p := Dequeue(F3);
            InsertLP(p);   // F3 → LP
        end;
    end;

    WriteLn('=== GAME OVER ===');
    DisplayState(F, F1, F3);

    PrintTopWinners;
end.
        