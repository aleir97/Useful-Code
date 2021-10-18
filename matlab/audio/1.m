clear;

[x,fs] = audioread('hola_22050.wav');
x = x';    lx = length(x);
n = 0 : 1/fs : (lx/fs)-1/fs;

a = 0.8;
h1 =  (a .^ n);  % Respuesta al impulso

h2 = 0;
for k = 0 : 0.05: 0.5
h2 = h2 + a*(n==k);
end

h = h1.*h2;

y = conv(x,h);              % Senal captada
ly = length(y);  
ny  = 0:1/fs:(ly/fs)-1/fs;

figure(1);
subplot(311);
plot(n,x); title('x(n)');
subplot(312);

stem(n, h); title('respuesta al impulso h(n)');
axis ([0 0.5 0 1]);
subplot(313);

plot(ny,y); title('y(n)');

rebotes = sum(abs(h)>0.001)
sound(y, fs);

