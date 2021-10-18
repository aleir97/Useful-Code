clear;

x = [1 1];
h = [0 0 0.5*x];
lx = length(x); nx = 0:lx-1;
lh = length(h); nh = 0:lh-1;

y = conv(x, h);
ly= (lx + lh - 1 );
ny = 0: ly-1;

figure(1);
subplot(311);
stem(nx, x);
subplot(312);
stem(nh, h);
subplot(313);
stem(ny, y);

% convolucion lineal

clear;

x = [1 0.5 0.25];
h = x;
lx = length(x); nx = 0:lx-1;
lh = length(h); nh = 0:lh-1;

y = conv(x, h);
ly= (lx + lh - 1 );
ny = 0: ly-1;

figure(2);
subplot(311);
stem(nx, x);
subplot(312);
stem(nh, h);
subplot(313);
stem(ny, y);

% circular
clear;

x = [1 0.5 0.25];
h = x;
x = [x x x];
lx = length(x); nx = 0:lx-1;
lh = length(h); nh = 0:lh-1;

y = conv(x, h);
ly= (lx + lh - 1 );
ny = 0: ly-1;

figure(3);
subplot(311);
stem(nx, x);
subplot(312);
stem(nh, h);
subplot(313);
stem(ny, y);

% zero - padding
clear;

x = [1 0.5 0.25];
h = x;
x = [x 0 0];
x = [x x x];

lx = length(x); nx = 0:lx-1;
lh = length(h); nh = 0:lh-1;

y = conv(x, h);
ly= (lx + lh - 1 );
ny = 0: ly-1;

figure(4);
subplot(311);
stem(nx, x);
subplot(312);
stem(nh, h);
subplot(313);
stem(ny, y);

% DFT
clear;

N = 1000;
n = 0 : N-1;    
%x = ones(1, N);
%x = (n == 0);
x = (n >= 0) & (n <= 100);

[X, W] = dtft(x,N);

figure(5);
subplot(411);
stem(n, x);

subplot(412);
stem(W, X);

subplot(413);
stem(W, abs(X));

subplot(414);
stem(W, 20*log10(abs(X + eps)));



