import numpy as np
import cv2

# Adjust dimensions if your frame is not 320x240
frame = np.fromfile("frame.yuyv", dtype=np.uint8).reshape((240, 320, 2))
bgr = cv2.cvtColor(frame, cv2.COLOR_YUV2BGR_YUYV)
cv2.imshow("Captured", bgr)
cv2.waitKey(0)
cv2.destroyAllWindows()
