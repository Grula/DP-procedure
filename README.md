Davis-Putnam Procedure (DP)
The Davis-Putnam procedure (DP) is a technique for deciding the satisfiability of propositional formulas in clausal form. The procedure shown here is not the Davis-Putnam-Longemann-Loveland (DPLL) procedure which is a later development.

Algorithm
The following three rules are applied until they can be applied no more:

The 1-literal rule: Any clause that contains a single literal is removed along with any other clause that contains this literal. The negation of this literal is then removed from all clauses. This is also known as unit propagation.

The affirmative-negative rule: The set of all literals that occur either only positively or negatively is calculated. These literals are called pure. Any clause that contains a pure literal is deleted.

The resolution rule: A literal is chosen that occurs only positively in one clause and only negatively in another clause. We then form all resolvents for this literal and combine the result with the clauses that were not used in the resolution.
