/* Run swipl sokoban.pl and later level3(X).                        */

:-include(sokoban).

level1(Solution):-
    solve([[],
           [right(x1y1, x2y1), right(x2y1, x3y1)],
           [box(x2y1)],
           [solution(x1y1)],
           sokoban(x3y1)],
           Solution).


level3(Solution):-
    solve([[top(x1y1,x1y2),top(x1y2,x1y3),top(x2y1,x2y2),top(x2y2,x2y3),top(x3y1,x3y2),top(x3y2,x3y3)],
           [right(x1y1,x2y1),right(x1y2,x2y2),right(x1y3,x2y3),right(x2y1,x3y1),right(x2y2,x3y2),right(x2y3,x3y3)],
           [box(x2y2)],
           [solution(x1y1)],
           sokoban(x1y3)],
           Solution).
