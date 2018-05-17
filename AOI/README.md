基于十字链表实现的AOI服务

![](https://travis-ci.org/pojol/aoi.svg?branch=master)


## Depend
* c++1y


## Sample
```c++
/* algorithm

	//std::set<int> old_set = { 1, 3, 5, 7 };
	//std::set<int> new_set = { 3,5,4 };

	//set_intersection 3,5
	//set_difference new_set, intersection_set { 4 }	= enter
	//set_difference old_set, intersection_set { 1, 7 } = leave
*/

/* sample output
    object 1 enter 2 range
    object 2 enter 1 range
    2 send 1 move pos change 5,5
    2 send 1 move pos change 5,6
    object 2 leave 1 range
    object 1 leave 2 range
    object 1 enter 2 range
    object 2 enter 1 range
    2 send 1 move pos change 4,5
    2 send 1 move pos change 5,4
    object 1 enter 3 range
    object 3 enter 1 range
    object 2 enter 3 range
    object 3 enter 2 range
    3 send 1 move pos change 4,4
    3 send 2 move pos change 4,4
*/
```


## License

MIT License

Copyright (c) 2017 pojol
