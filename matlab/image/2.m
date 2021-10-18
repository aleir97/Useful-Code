clear;

%Leemos las imagenes 
Io = imread('circuit.tif'); %uint8
Id = double(imread('circuit.tif')); %double

%imagesc(Id); colormap(gray); axis off;	

N= 256;
Im= Id(1:N,1:N);

%p= 0.05;
%U= rand(N); 
%Iruido= Im.*(U>p) + 255*(U<=p); 

d= 50;
m= 0;
R= sqrt(d)*randn(N)+m;
Iruido= Im+R;

L= 5; %tamaño de la máscara
La= fix(L/2);
I= [zeros(La,N); Iruido; zeros(La,N)];
I= [zeros(N+2*La,La) I zeros(N+2*La,La)];

W(1:L, 1:L)= 1/(L^2); 

La=fix(L/2);
for k=La+1:N
    for m=La+1:N
        X=I(k-La:k+La,m-La:m+La);
        
        %Y = median(X(:));
        Y= sum(sum(X.*W));
      	 
        Ir(k,m)=Y;
    end;
end;

figure(); 
subplot(221); imagesc(I); 
colormap(gray); axis off; %subtitle('Original')
subplot(222); imagesc(Ir); subtitle('filtro media 5x5') 