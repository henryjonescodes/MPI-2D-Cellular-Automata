<h2> <center> Week 5 Project: 2D Cellular Automota </h2>
<h3> <center> Henry Jones & Theo Seidel</h3>


<h3>  Contributions:</h3>

We both endeavored to write a lot of overlapping functions, as we were both confused about how to go about the project at the beginning. At the end of the day a lot of the functions used were written by Henry, but as mentioned in ‘coordination’ we were constantly collaborating in person or over the internet. Reciprocativly, Theo took charge of the timing data and we wrote the writeup in google drive together before adding to the repo.


<h3>  Coordination:</h3>

Almost the entire project was written with both of us in the same room working together, as such there was less need to use git to collaborate. We’re friends and communication was already in place, remotely we spent a few zoom meetings working on the project and chatted over discord. In general the project wasn’t a code crunch, we would work on a section and get it working and take some time in between to work on other coursework.



<h3>  Timing Results:</h3>


|  # p	|  100	|  500	| 600 	|  700	|  800	| 900	|
|-	|-	|-	|-	|-	|-	|-	|
| 1 	| 0.002162 s 	|  0.050980 s	|  0.072478 s	|  0.096909 s	|  	0.125611 s|  0.160828 s	|
| 2 	|  0.001798 s	|  0.026498 s	|   0.037725 s	| 0.050818 s 	|  	0.065978 s| 0.083209 s 	|
| 4 	|  0.001045 s	|  0.013592 s	| 0.021085 s 	|  0.027088 s	|  0.036203 s	|  0.043038 s	|
| Speedup (2 Cores) | 1.202447164| 1.923918786| 1.921219351| 1.906981778|1.90383158|1.932819767
| Speedup (4 Cores) | 2.068899522|3.750735727 | 3.437419967| 3.57756202|3.469629589 | 3.736883684
|Average Speedup (2Cores)| 1.798536404
|Average Speedup (4Cores)| 3.340188418

The results show consistent speedup for all world sizes using 2 and 4 cores. The average speedup for 4 cores was rougly double that of 2 cores suggesting that the program scales well for parallel computing. 
