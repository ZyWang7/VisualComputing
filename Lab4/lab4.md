#### Aim: Detecting the horizon in images.

1. Convert the image into greyscale (if necessary).
2. Apply a Canny filter on the image.
3. Apply a probabilistic Hough transformation that will return a list of pairs of Points defining the start and end coordinates for line segments.
4. Filter out the short lines, use Pythagoras to compute the lines’ lengths.
5. Filter out the vertical lines. You could do that by either calculating the inverse tangent of each line (use atan2), finding its angle from the horizontal, or check whether the x co-ordinates of the segment’s endpoints are similar.
6. Polynomial regression. draw a curve that best fits all those (nearly) horizontal lines’ points. It takes some points and calculates the best polynomial of any order that you choose that fits all the points. Be careful not to overfit the points though; since the horizon curve best matches a quadratic function choosing a higher order polynomial can give you unstable results, i.e. a very wavy line.