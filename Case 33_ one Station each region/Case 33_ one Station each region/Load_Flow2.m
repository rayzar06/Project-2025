function Load_Flow2
format short;
tic
LC=[2 19 20 21 22 22 22 22 22   
    3 23 24 25 25 25 25 25 25
    4 5 6 26 27 28 29 31 33
    9 10 11 12 12 12 12 12 12
    13 14 15 16 17 18 18 18 18]; % Bus of Each Region 
for k=1:5
EV(k)=LC(k,randi([1,9],1));
end
Num_EV=5;
% m=load('loaddata33bus.m');
% l=load('linedata33bus.m');
PS=zeros(33,1);
QS=zeros(33,1);

for n=1:Num_EV
        Ps=[133.2 54.6 149.7 54.6 129.9];
        Qs=[84.1  34.5 94.5  34.5 82];
        PS(round(EV(n),0))=PS(round(EV(n),0))+Ps(n);
        QS(round(EV(n),0))=QS(round(EV(n),0))+Qs(n);
    end
m=load('loaddata33bus.m');
l=load('linedata33bus.m');

br=length(l);
no=length(m);
f=0;
d=0;
MVAb=100;
KVb=12.66;
Zb=(KVb^2)/MVAb;
% Per unit Values
for i=1:br
    R(i,1)=(l(i,4))/Zb;
    X(i,1)=(l(i,5))/Zb;
end
for i=1:no
    P(i,1)=((m(i,2)+PS(i))/(1000*MVAb));
    Q(i,1)=((m(i,3)+QS(i))/(1000*MVAb));
end
R;
X;
P;
Q;
C=zeros(br,no);
for i=1:br
    a=l(i,2);
    b=l(i,3);
    for j=1:no
        if a==j
            C(i,j)=-1;
        end
        if b==j
            C(i,j)=1;
        end
    end
end
C;
e=1;
for i=1:no
    d=0;
    for j=1:br
        if C(j,i)==-1
            d=1;
        end
    end
    if d==0
        endnode(e,1)=i;
        e=e+1;
    end
end
endnode;
h=length(endnode);
for j=1:h
    e=2;
    
    f=endnode(j,1);
   % while (f~=1)
   for s=1:no
     if (f~=1)
       k=1;  
       for i=1:br
           if ((C(i,f)==1)&&(k==1))
                f=i;
                k=2;
           end
       end
       k=1;
       for i=1:no
           if ((C(f,i)==-1)&&(k==1));
                f=i;
                g(j,e)=i;
                e=e+1;
                k=3;
           end            
       end
     end
   end
end
for i=1:h
    g(i,1)=endnode(i,1);
end
g;
w=length(g(1,:));
for i=1:h
    j=1;
    for k=1:no 
        for t=1:w
            if g(i,t)==k
                g(i,t)=g(i,j);
                g(i,j)=k;
                j=j+1;
             end
         end
    end
end
g;
for k=1:br
    e=1;
    for i=1:h
        for j=1:w-1
            if (g(i,j)==k) 
                if g(i,j+1)~=0
                    adjb(k,e)=g(i,j+1);            
                    e=e+1;
                else
                    adjb(k,1)=0;
                end
             end
        end
    end
end
adjb;
for i=1:br-1
    for j=h:-1:1
        for k=j:-1:2
            if adjb(i,j)==adjb(i,k-1)
                adjb(i,j)=0;
            end
        end
    end
end
adjb;
x=length(adjb(:,1));
ab=length(adjb(1,:));
for i=1:x
    for j=1:ab
        if adjb(i,j)==0 && j~=ab
            if adjb(i,j+1)~=0
                adjb(i,j)=adjb(i,j+1);
                adjb(i,j+1)=0;
            end
        end
        if adjb(i,j)~=0
            adjb(i,j)=adjb(i,j)-1;
        end
    end
end
adjb;
for i=1:x-1
    for j=1:ab
        adjcb(i,j)=adjb(i+1,j);
    end
end
b=length(adjcb);

% voltage current program

for i=1:no
    vb(i,1)=1;
end
for s=1:10
for i=1:no
    nlc(i,1)=conj(complex(P(i,1),Q(i,1)))/(vb(i,1));
end
nlc;
for i=1:br
    Ibr(i,1)=nlc(i+1,1);
end
Ibr;
xy=length(adjcb(1,:));
for i=br-1:-1:1
    for k=1:xy
        if adjcb(i,k)~=0
            u=adjcb(i,k);
            %Ibr(i,1)=nlc(i+1,1)+Ibr(k,1);
            Ibr(i,1)=Ibr(i,1)+Ibr(u,1);
        end
    end      
end
Ibr;
for i=2:no
      g=0;
      for a=1:b 
          if xy>1
            if adjcb(a,2)==i-1 
                u=adjcb(a,1);
                vb(i,1)=((vb(u,1))-((Ibr(i-1,1))*(complex((R(i-1,1)),X(i-1,1)))));
                g=1;
            end
            if adjcb(a,3)==i-1 
                u=adjcb(a,1);
                vb(i,1)=((vb(u,1))-((Ibr(i-1,1))*(complex((R(i-1,1)),X(i-1,1)))));
                g=1;
            end
          end
        end
        if g==0
            vb(i,1)=((vb(i-1,1))-((Ibr(i-1,1))*(complex((R(i-1,1)),X(i-1,1)))));
        end
end
s=s+1;
end
nlc;
Ibr;
vb;
vbp=[abs(vb) angle(vb)*180/pi];


for i=1:no
vbp(i,1)=abs(vb(i));
vbp(i,2)=angle(vb(i))*(180/pi);
end


toc;
for i=1:no
    va(i,2:3)=vbp(i,1:2);
end
for i=1:no
    va(i,1)=i;
end
va;


Ibrp=[abs(Ibr) angle(Ibr)*180/pi];
PL(1,1)=0;
QL(1,1)=0;

% losses
for f=1:br
    Pl(f,1)=(Ibrp(f,1)^2)*R(f,1);
    Ql(f,1)=X(f,1)*(Ibrp(f,1)^2);
    PL(1,1)=PL(1,1)+Pl(f,1);
    QL(1,1)=QL(1,1)+Ql(f,1);
end

Plosskw=(Pl)*100000;
Qlosskw=(Ql)*100000;
PL=(PL)*100000;
QL=(QL)*100000;


voltage = vbp(:,1);
Vb=abs(voltage);
%angle = vbp(:,2)*(pi/180);

% hold on

% Plosskw
sum(Plosskw );
sum(Qlosskw);

%
Plosskw1(33,1)=PL;
Qlosskw2(33,1)=QL;

%sprintf('Power-Loss=%d KW, Power-Loss=%d KVAr' ,PL,QL')     
Sr=(1:33)';
figure
plot(m(:,1),abs(voltage))
xlabel('Bus'),ylabel('p.u'),title('Voltage Profile')

figure
plot(l(:,1),Plosskw)
xlabel('Bus'),ylabel('Kw'),title('Power Loss')

VD=sum((1-Vb).^2)*100;
disp('                            ')
disp('----------------------------------------------')
disp('         Optimal Location EV Station          ')
disp('----------------------------------------------')
disp(['Power-Loss (KW):                           ' num2str(round(PL,0))]);
disp(['Power-Loss (KVar):                         ' num2str(round(QL,0))]);
disp(['Voltage Deviation (%):                     ' num2str(VD)]);
disp('-----------------------------------------------');
    %% EXCEL FOR DG
% %EXCEL
% T =table(Sr,Plosskw,Qlosskw,angle,voltage);        
% T(:,1:5);
% excel_file = 'NO_DG_IEEE33.xlsx';
% writetable(T,excel_file,'Sheet',1,'Range','H1');

