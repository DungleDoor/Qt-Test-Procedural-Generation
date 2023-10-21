# Qt-Test-Procedural-Generation

### Instructions
Just clone the repository and open in qtcreator
it should work, but i used a slightly different version of qt so if it doesn't just make a blank project and copy the code over

### Settings
* Noise Percentage - initial percent of cells that are alive (white)
* Generations - # of generations rules are applied, set to 0 to see initial noise pattern
* The last two define the two rules that are applied to generate map, should be pretty self explanatory, overpopulation is set to 9 because there can never be more than 8 alive neighbors
* you can play with the size of each square and total map size in mainwindow.h, it shouldn't break anything (hopefully)
* there is also a minimum map distance parameter you can mess with in mainwindow.h

#### Note 
* startpoint in lower left area green, endpoint is blue 
* any unreachable areas are set to red
