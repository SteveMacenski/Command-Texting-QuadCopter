clear all
clc

url = 'http://172.17.193.164:8080/shot.jpg';
 
ri  = imread(url);
fh = image(ri);
    fname = 'Video.avi';
    v = VideoWriter(fname);
    v.Quality = 100;
    open(v)

tic
while toc<=10
    ri = imread(url);
	set(fh,'CData',ri);
    writeVideo(v,ri)

    drawnow;  
end

     close(v)
