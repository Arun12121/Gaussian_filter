from pickletools import uint8
import numpy as np
from PIL import Image
import numpy as np
import cv2

img = np.loadtxt("out.txt", dtype=int)
print(img)
print(np.min(img))

disp = Image.fromarray(np.uint8(img), 'L')
print(np.asarray(disp).astype(int))
disp.show()
disp.save("test/test2o.jpg")
