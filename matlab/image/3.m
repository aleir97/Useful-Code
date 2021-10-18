clear;

%Leemos las imagenes 
Id = double(imread('barbara.tif')); %double

d= 1000;
m= 10;
R= sqrt(d)*randn(512)+m;
Id= Id+R;

If = fft2(Id);
F = fftshift(If); % Center FFT



% [sX,sY]= size(Id);
% Index_X = -(sX/2):(sX/2 -1);
% u = Index_X' * ones(1,sY) ;
% Index_Y = -(sY/2):(sY/2 -1);
% v = ones(sY,1) * Index_Y ;
% Duv = sqrt(u.^2 + v.^2);
% 
% D0 = 16;
% % Low Pass Filter 
% Low_Filter= Duv <= D0;
%  
% %High Pass Filter
% High_Filter= Duv > D0;

[sX,sY]= size(Id);
Index_X = -(sX/2):(sX/2 -1);
u = Index_X' * ones(1,sY) ;
Index_Y = -(sY/2):(sY/2 -1);
v = ones(sY,1) * Index_Y ;
Duv = sqrt(u.^2 + v.^2);

n= 512;
D0 = 64;
% Low Pass Filter 
Low_Filter= 1 ./ (1.0 + (Duv ./ D0).^(2*n));
 
%High Pass Filter
High_Filter= 1-Low_Filter;

Fi = Low_Filter .* F;
Ir = real(ifft2(fftshift(Fi)));

figure();
subplot(421); imagesc(Id);  
colormap(gray);  title('original');
subplot(422); imagesc(100*log(1+abs(F))); 
colormap(gray);  title('magnitude spectrum original');

subplot(423); imagesc(100*log(1+abs(Fi))); 
colormap(gray);  title('magnitude spectrum filtered');

subplot(424); imagesc(Ir); 
colormap(gray);  title('ifft image D0=64');


% figure;  
% subplot(421); imagesc(100*log(1+abs(F))); 
% colormap(gray);  title('magnitude spectrum');
% subplot(422); imagesc(angle(F));  	
% colormap(gray); title('phase spectrum');
% 
% Ir = ifft2(fftshift(F));
% 
% subplot(423); imagesc(Id);  
% colormap(gray);  title('original');
% 
% subplot(424); imagesc(Ir); 
% colormap(gray); title('recuperada');
% 



