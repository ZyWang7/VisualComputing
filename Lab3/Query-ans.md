# Lab3 Query

## 1. Is Otsu’s method successful in thresholding all the images?

No, not all of them.

 - **fundus.tif**

   Whole eye is identified, but no blood vessels can be identified, doesn't give a good result.

 - **glaucoma.jpg**

   Whole eye is identified, but the region cannot be identified. A good result could not be obtained.

 - **optic nerve head.jpg**

   The outline of the large, slightly brighter region seems to be found, but the outline of the smaller, brighter region within it is not identified.

 - **motorway.png**

   The white text is clearly in the output. Success.

   

## 2. How would you modify the thresholding algorithm to address any problems?

For `fundus.tif` and `glaucoma.jpg`, only the entire eye is recognised. This is because the background of the original image is black, while the eye are of a different kind of red of a similar colour. The Otsu Method will automatically classifies the black background as one category and the whole eye as another, thus losing the detail of the eye.

To solve this problem, we can **divide the image into smaller sections**, separating the eyes from the black background. Apply the Otsu method to each part and combine the different parts together to get the final output.

A good example of this is `optic nerve head.jpg`, which is an image of a part of the eye. And that identifies the different areas well.

Alternatively, we can **set multiple threshold values**. For example, one threshold for separating the black background from the eyes, and another for identifying the parts we want.




## 3. What metrics are there for assessing the success of thresholding?

Compare the threshold output image with **the ground truth** or **the expected result**. 

- **Accuracy**

  Accuracy is the ratio of the number of correctly classified pixels to the total number of pixels in the image. High accuracy means that the algorithm is correctly classifying a high percentage of all instances.

- **Precision and Recall**

  **Precision** is the ratio of true positive pixels to the total number of pixels classified as positive. High precision means it can accurately identifies the foreground pixels (pixels of interest) while minimizing the number of false positive pixels (pixels not of interest) in the output image.

  **Recall** is the ratio of true positive pixels to the total number of actual positive pixels. High recall means that the algorithm is correctly identifying a high percentage of all positive instances.

- **F1 Score**

  It is the harmonic mean of precision and recall.  

  $$ F1 Score = { 2 * precision * recall \over precision + recall} $$

  High F1 score means that the algorithm has a good balance between precision and recall, and is performing well overall.