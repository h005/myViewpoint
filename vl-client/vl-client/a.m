I = imread('lugger1.jpg');
imshow(I);
I = single(rgb2gray(I));
[f, d] = vl_sift(I);

perm = randperm(size(f,2)) ;
sel = perm(1:50) ;
h1 = vl_plotframe(f) ;
h2 = vl_plotframe(f) ;
set(h1,'color','k','linewidth',3) ;
set(h2,'color','y','linewidth',2) ;