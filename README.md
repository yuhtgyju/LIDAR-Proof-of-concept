Press SPACE to do a lidar scan, arrow keys to move, a/d to turn, b to toggle blindness.

At line 186 at this segment: for (float t = 0; t < 1.0f; t += 0.0005f) you can change the 0.0005f to be any number you desire, it's the steps that the vectors take before detecting collision, too high and results may look strange.
the lower the number the slower the game, but the better the results, the higher the number the faster the game, but the worse the results.
