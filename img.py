import numpy as np
from PIL import Image, ImageOps, ImageFilter
from scipy.ndimage import gaussian_filter
img = Image.open("test2in.jpg")
img = ImageOps.grayscale(img)
img = img.resize((360,480))
img.show()
img = np.asarray(img)
print(img)
np.savetxt("img.txt", img.astype(int), fmt="%i")
img = gaussian_filter(img, sigma=1)
disp = Image.fromarray(np.uint8(img), 'L')
disp.show()
disp.save("test/test2o_actual.jpg")