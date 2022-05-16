%Gaussian filter using MATLAB built_in function
%Read an Image
tic
Img = imread('test1in.jpg');
%A = imnoise(Img,'Gaussian',0.04,0.003);
%Image with noise
%figure,imshow(Img);

H = fspecial('Gaussian',[5 5],1);
%h = gaussian2D([5 5], 1);
GaussF = imfilter(Img,H);
toc
figure,imshow(GaussF);