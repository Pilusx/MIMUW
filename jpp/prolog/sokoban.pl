solve(Problem, Solution):-
/***************************************************************************/
/* Your code goes here                                                     */
/* You can use the code below as a hint.                                   */
/***************************************************************************/
    
Problem = [Tops, Rights, Boxes, Solutions, sokoban(Sokoban)],
abolish_all_tables,
retractall(top(_,_)),
findall(_, ( member(P, Tops), assert(P) ), _),
retractall(right(_,_)),
findall(_, ( member(P, Rights), assert(P) ), _),
retractall(solution(_)),
findall(_, ( member(P, Solutions), assert(P) ), _),

retractall(initial_state(_,_)),
findall(Box, member(box(Box), Boxes), BoxLocs),
solve_handler(state(Sokoban, BoxLocs), Solution).

extract_move(push(_Box, Dir), Dir).
extract_move(move(_Loc, Dir), Dir).

solve_handler(X, Solution) :- 
    solve_problem(X, [X], Actions),
    maplist(extract_move, Actions, Solution).

solve_problem(State, History, NewMoves) :-
    (final_state(State), NewMoves = []);
    (
        movement(State, Move, SokobanMoves),
        update(State, Move, NewState),
        \+ member(NewState, History),
        solve_problem(NewState, [NewState|History], Moves),
        flatten([SokobanMoves, [Move], Moves], NewMoves)
    ).

:-include(game).
