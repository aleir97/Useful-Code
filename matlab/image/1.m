clear;

%Leemos las imagenes 
Io = imread('circuit.tif'); %uint8
Id = double(imread('circuit.tif')); %double

N= 256;
Im= Id(1:N,1:N);

L= 3; %tamaño de la máscara
La= fix(L/2);
I= [zeros(La,N); Im; zeros(La,N)];
I= [zeros(N+2*La,La) I zeros(N+2*La,La)];

%W(1:L, 1:L)= 1/(L^2); 
%W= [0,-1,0; -1,4,-1; 0,-1,0];
%W= [-1,-1,-1; -1,-8,-1; -1,-1,-1];

W= [-1,-1,-1; 0,0,0; 1,1,1];

La=fix(L/2);
for k=La+1:N
    for m=La+1:N
        X=I(k-La:k+La,m-La:m+La);
        
        %Y = median(X(:));
        Y= sum(sum(X.*W));
      	 
        If(k,m)=Y;
    end;
end;

W= [-1,0,1; -1,0,1; -1,0,1];

La=fix(L/2);
for k=La+1:N
    for m=La+1:N
        X=I(k-La:k+La,m-La:m+La);
        
        %Y = median(X(:));
        Y= sum(sum(X.*W));
      	 
        Ic(k,m)=Y;
    end;
end;

Ir = abs(If)+ abs(Ic);  
U = 90;

%Iu = (Ir<U)*0 + (Ir>=U)*255;
Iu = (Ir<U).*Im + (Ir>=U)*255;

figure(); 
subplot(321); imagesc(If); 
colormap(gray); 
subplot(322); imagesc(Ic);  
subplot(323); imagesc(Iu); subtitle('U = 90');

