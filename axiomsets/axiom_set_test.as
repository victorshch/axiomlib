axiom_set_test
axiombase

1: axiom_test_1
	derivative_decrease_left:			derivDecreaseMinLevel -> 0.05
	derivative_decrease_left:			derivDecreaseMaxLevel -> 0.2
	derivative_decrease_left:			derivDecreaseLeft -> 3
	second_derivative_decrease_left:	secDerivDecreaseMinLevel -> 0.1
	second_derivative_decrease_left:	secDerivDecreaseMaxLevel -> 0.7
	second_derivative_decrease_left: 	secDerivDecreaseLeft -> 4

2: axiom_test_2
	derivative_decrease_left:			derivDecreaseMinLevel -> 0.05
	derivative_decrease_left:			derivDecreaseMaxLevel -> 0.2
	derivative_decrease_left:			derivDecreaseLeft -> 3
	second_derivative_increase_left:	secDerivIncreaseMinLevel -> 0.1
	second_derivative_increase_left:	secDerivIncreaseMaxLevel -> 0.7
	second_derivative_increase_left: 	secDerivIncreaseLeft -> 4

3: axiom_test_3
	derivative_increase_left:			derivIncreaseMinLevel -> 0.05
	derivative_increase_left:			derivIncreaseMaxLevel -> 0.2
	derivative_increase_left:			derivIncreaseLeft -> 3
	second_derivative_decrease_left:	secDerivDecreaseMinLevel -> 0.1
	second_derivative_decrease_left:	secDerivDecreaseMaxLevel -> 0.7
	second_derivative_decrease_left: 	secDerivDecreaseLeft -> 4

4: axiom_test_4
	derivative_increase_left:			derivIncreaseMinLevel -> 0.05
	derivative_increase_left:			derivIncreaseMaxLevel -> 0.2
	derivative_increase_left:			derivIncreaseLeft -> 3
	second_derivative_increase_left:	secDerivIncreaseMinLevel -> 0.1
	second_derivative_increase_left:	secDerivIncreaseMaxLevel -> 0.7
	second_derivative_increase_left: 	secDerivIncreaseLeft -> 4

5: axiom_test_5
	derivative_decrease_left:			derivDecreaseMinLevel -> 0.05
	derivative_decrease_left:			derivDecreaseMaxLevel -> 0.2
	derivative_decrease_left:			derivDecreaseLeft -> 3

6: axiom_test_6
	derivative_increase_left:			derivIncreaseMinLevel -> 0.05
	derivative_increase_left:			derivIncreaseMaxLevel -> 0.2
	derivative_increase_left:			derivIncreaseLeft -> 3
