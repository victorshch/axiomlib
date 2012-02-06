Testing
no_name_AxiomBank

1: no_name
			decrease_right_elementary_condition:		decreaseLeft -> 0.000000
			decrease_right_elementary_condition:		decreaseMaxLevel -> 10.473712
			decrease_right_elementary_condition:		decreaseMinLevel -> 1.124191
			decrease_right_elementary_condition:		decreaseRight -> 1.000000
			increase_left_elementary_condition:		increaseLeft -> 1.000000
			increase_left_elementary_condition:		increaseMaxLevel -> 10.560044
			increase_left_elementary_condition:		increaseMinLevel -> 1.519742
			increase_left_elementary_condition:		increaseRight -> 0.000000

2: no_name
			derivative_decrease_right:		derivDecreaseLeft -> 0.000000
			derivative_decrease_right:		derivDecreaseMaxLevel -> 9.828409
			derivative_decrease_right:		derivDecreaseMinLevel -> 1.375796
			derivative_decrease_right:		derivDecreaseRight -> 2.000000
	not: 	increase_right_elementary_condition:		increaseLeft -> 0.000000
	not: 	increase_right_elementary_condition:		increaseMaxLevel -> 9.201144
	not: 	increase_right_elementary_condition:		increaseMinLevel -> 1.016319
	not: 	increase_right_elementary_condition:		increaseRight -> 1.000000
	or
			derivative_decrease_left:		derivDecreaseLeft -> 3.000000
			derivative_decrease_left:		derivDecreaseMaxLevel -> 0.508494
			derivative_decrease_left:		derivDecreaseMinLevel -> 0.092036
			derivative_decrease_left:		derivDecreaseRight -> 0.000000
	or
			second_derivative_decrease_left:		secDerivDecreaseLeft -> 4.000000
			second_derivative_decrease_left:		secDerivDecreaseMaxLevel -> 0.378553
			second_derivative_decrease_left:		secDerivDecreaseMinLevel -> 0.052327
			second_derivative_decrease_left:		secDerivDecreaseRight -> 0.000000

3: axiom_test_1
	not: 	derivative_decrease_left:		derivDecreaseLeft -> 3.000000
	not: 	derivative_decrease_left:		derivDecreaseMaxLevel -> 0.508494
	not: 	derivative_decrease_left:		derivDecreaseMinLevel -> 0.092036
	not: 	derivative_decrease_left:		derivDecreaseRight -> 0.000000
			second_derivative_decrease_left:		secDerivDecreaseLeft -> 4.000000
			second_derivative_decrease_left:		secDerivDecreaseMaxLevel -> 0.378553
			second_derivative_decrease_left:		secDerivDecreaseMinLevel -> 0.052327
			second_derivative_decrease_left:		secDerivDecreaseRight -> 0.000000
