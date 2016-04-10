function [alpha_A,alpha_B,u_N]=mc_svm_2k_alcg_ls2(KA,KB,Y,CA,CB,D,eps,ikernel,ifeature)
%function [alpha_A,alpha_B,beta_P,beta_N]=mc_svm_2k_alcg_ls(XA,XB,Y,CA,CB,D,eps,ikernel)
% implemetation of the solver svm_2k
% optimization method: augmented lagrangian on the equality constraints
% 
% input:
%           KA          first feature matrix, the rows correspond to the items the columns
%                       correspond to the features
%           KB          second feature matrix, the rows correspond to the items the columns
%                       correspond to the features
%           Y           label vector
%           CA          penalty factor of the slacks in the first SVM subproblem
%           CB          penalty factor of the slacks in the second SVM subproblem
%           D           penalty factor on the interaction
%           eps         tolerance
%           ikernel     kernel type
%           ifeature    =1 input is feature =0 kernel directly
%
% output:
%           alpha_A     dual solution of the first SVM
%           alpha_B     dual solution of the second SVM
%           u_N         beta_+ -beta_- the difference of dual variables of the synthesis constraints

% ****************************************************
cgrowmax=2;     % growing factor of penalty parameter
ck=1;       % first penalty
maxiter=10;    % maximum iteration of the lagrangian
maxiter2=500;   % maximum iteration of z

eps_lambda=0.01;         % control parameter to avoid early stop
eps_z=0.0001;

ilambda=1;   % the lagrangian is considered

tau=1;       % maximum step size in the subproblem
xtau=(1+sqrt(5))/2-1;
cgrow=cgrowmax;

linesearch=0;
epstau=0.0000001;
iident=0;
identw=1;
ireweight=1;
% ****************************************************
lambda=[0;0];   % intial values of the Lagrangian
% ****************************************************

if ifeature==1

    [m,n]=size(KA);
    e1=ones(m,1);
    e2=e1*e1';

% ikernel=1;

    KA=KA*KA';  % linear kernel
    KB=KB*KB';
    switch ikernel 
        case 0
        case 1 % polynomial
            KA=(KA+e2).^2;
            KB=(KB+e2).^2;
        case 2 % sigmoid
            KA=tanh(KA+e2);
            KB=tanh(KB+e2);
        case 3  % Gaussian
            sigma=n;
            dd=diag(KA);
            KA=dd*e1'+e1*dd'-2*KA;
            KA=exp(-KA/sigma);

            dd=diag(KB);
            KB=dd*e1'+e1*dd'-2*KB;
            KB=exp(-KB/sigma);
    end
else
    [m,n]=size(KA);
    e1=ones(m,1);
    e2=e1*e1';
end  

h=zeros(2,1);

% **************************

% **************************


xgr1=zeros(maxiter2,maxiter);

% variables for diagnostic purpose
xlerr=zeros(maxiter,1);
%xherr=zeros(maxiter,maxiter2,2);
xherr=zeros(maxiter,1);
xckerr=zeros(maxiter,1);
xstep=zeros(maxiter,maxiter2);

nz=3;

zz.z=zeros(nz*m,1);          % value of the variables
zz.lB=[zeros((nz-1)*m,1);-D*e1];         % lower bound    
%zz.lB=[zeros((nz-1)*m,1);zeros(m,1)];         % lower bound    

if ireweight==0
  zz.uB=[CA*e1;CB*e1;D*e1]; % absolut upper bound
else
  yp=sum(Y==1);
  yn=sum(Y~=1);
  ww=e1;
  ww(find(Y~=1))=sqrt(yp/yn);
  zz.uB=[CA*ww;CB*ww;D*ww]; % absolut upper bound
end
%zz.uB=[CA*e1;CB*e1;zeros(m,1)]; % absolut upper bound


% initial values
zz.z(1:m)=CA/2;
zz.z(m+1:2*m)=CB/2;
zz.z((nz-1)*m+1:nz*m)=0;


ga=zeros(m,1);
gb=zeros(m,1);

grzQ0=zeros(nz*m,1);
grzQ1=zeros(nz*m,1);
grznew0=zeros(nz*m,1);

grzQ=zeros(nz*m,1);
grznew=zeros(nz*m,1);
gr0=-realmax;
herr0=realmax;

s0=zeros(nz*m,1);
s1=zeros(nz*m,1);
s=zeros(nz*m,1);

YY=sparse(diag(Y));

% constant, linear part of the objective function
    s0(1:m)=-e1;
    s0(m+1:2*m)=-e1;
    s0(2*m+1:3*m)=0;
% linear part of the objective depending on lambda 
    s1(1:m)=-lambda(1)*Y;
    s1(m+1:2*m)=-lambda(2)*Y;
    s1(2*m+1:3*m)=+(lambda(1)-lambda(2))*e1;

    s=s0+s1;

% loop for the lagrangian iteration

% variables for diagnostic purpose
ffx=zeros(maxiter2,maxiter);
fffx=zeros(maxiter2,maxiter);
taux=zeros(maxiter2,maxiter);
ffmin=realmax;
fffmin=realmax;
zmin=zeros(nz*m,1);

% set up for the outer loop, which is the augmented lagrangian loop
lambda0=lambda;
ck0=ck;

herr=realmax;

% outer loop maximization on the lagrangien
for k=1:maxiter
    zz0=zz.z;
% iteration to solve the subproblem


    grz=zeros(nz*m,1);
    gr0=-realmax;

% if lambda is considered we need to recompute the linear part    
    if ilambda==1
        s0(1:m)=-e1;
        s0(m+1:2*m)=-e1;
        s0(2*m+1:3*m)=0;

        s1(1:m)=-lambda(1)*Y;
        s1(m+1:2*m)=-lambda(2)*Y;
        s1(2*m+1:3*m)=+(lambda(1)-lambda(2))*e1;

        s=s0+s1;
    end

% computation of the gradient    
    ga=Y.*zz.z(1:m)-zz.z(2*m+1:3*m);
    gb=Y.*zz.z(m+1:2*m)+zz.z(2*m+1:3*m);

    
    if iident==1
        kaga=(KA+identw*speye(m))*ga;
        kbgb=(KB+identw*speye(m))*gb;
    else
        kaga=KA*ga;
        kbgb=KB*gb;
    end
    ega=e1'*ga;
    egb=e1'*gb;
    egu=zz.z(2*m+1:3*m);
% quadratic part independent from the running constant ck        
    grzQ0(1:m)=Y.*kaga;
    grzQ0(m+1:2*m)=Y.*kbgb;
    grzQ0(2*m+1:3*m)=kbgb-kaga+2*eps*egu;

% quadratic part dependent on the running constant ck        
    grzQ1(1:m)=ck*Y*ega;
    grzQ1(m+1:2*m)=ck*Y*egb;
    grzQ1(2*m+1:3*m)=ck*(egb-ega)*e1;
        
    grzQ=grzQ0+grzQ1;
    grz=grzQ+s;
    
% solution of the linear subproblem for finding the best primal at a given
% and fixed dual 
% applied method, so called, conditional gradient method
    for irepeat=1:maxiter2
        
        z0=zz.z;

% solve the subproblem on the box constraints to find the next iteration         
        znew=zz.lB;
        igr=(grz(1:nz*m)<0);
        znew=znew+(zz.uB-zz.lB).*igr;
        
        
        dstep=znew-z0;

% the value of the gradient in the current z        
        
        gad=Y.*znew(1:m)-znew(2*m+1:3*m);
        gbd=Y.*znew(m+1:2*m)+znew(2*m+1:3*m);
        
%        plot(gad);hold on;plot(gbd,'r');hold off;
%        drawnow;
%        pause;

    if iident==1
        kagad=(KA+identw*speye(m))*gad;
        kbgbd=(KB+identw*speye(m))*gbd;
    else
        kagad=KA*gad;
        kbgbd=KB*gbd;
    end
        egad=e1'*gad;
        egbd=e1'*gbd;
        egud=znew(2*m+1:3*m);
        
        grznew0(1:m)=Y.*kagad;
        grznew0(m+1:2*m)=Y.*kbgbd;
        grznew0(2*m+1:3*m)=kbgbd-kagad+2*eps*egud;

        grznew(1:m)=ck*Y*egad;
        grznew(m+1:2*m)=ck*Y*egbd;
        grznew(2*m+1:3*m)=ck*(egbd-egad)*e1;
        
        grznew=grznew0+grznew;
        
        grzd0=grznew0-grzQ0;
        grzd=grznew-grzQ;
        
% the new approximation of the optimum solution        
        if linesearch==0
        
            dnscale=dstep'*grzd;
%        disp(dnscale);
            if dnscale<epstau;
                dnscale=epstau;
            end
            tau=-grz'*dstep/dnscale;
%            tau=0.5*tau;
            if tau<=0
                tau=epstau;
            end
            if tau>=1
                tau=1-epstau;
            end
%            disp(tau);
%            tau=1/irepeat;
            zz.z=z0+tau*dstep;  % (1-tau)*z0+tau*znew 
            if isnan(zz.z)==1
                disp(dnscale);
            end
        else
            tau=1;
            z1=z0+tau*dstep;    % (1-tau)*z0+tau*znew
            fff=z1'*(0.5*((1-tau)*grzQ+tau*grznew)+s);
        end
%        za=(dstep'*z0)/(sqrt(sum(dstep.^2))*sqrt(sum(z0.^2)));        
%        taux(irepeat,k)=za;
        
% ########################################
        

% ########################################

        

        
% update the gradient based on the optimum step size        
%        gr1=grz'*(zz.z-z0);
        gr1=grz'*dstep;
        gr0=gr1;

        
        
        xstep(k,irepeat)=gr1;
%        ldstep=tau*sum(dstep.^2);
%        format('short','g');

%        ga=Y.*zz.z(1:m)-zz.z(2*m+1:3*m)+zz.z(3*m+1:4*m);
%        gb=Y.*zz.z(m+1:2*m)+zz.z(2*m+1:3*m)-zz.z(3*m+1:4*m);
        ga=(1-tau)*ga+tau*gad;
        gb=(1-tau)*gb+tau*gbd;
        grzQ=(1-tau)*grzQ+tau*grznew;
        grzQ0=(1-tau)*grzQ0+tau*grznew0;
        grz=grzQ+s;

%        disp([min(ga),max(ga),min(gb),max(gb)]);
        
       
        h1=e1'*ga;
        h2=e1'*gb;
%        if mod(irepeat,10)==0
%            disp([h1,h2,gr1,ldstep,min(zz.z(1:2*m))]);
%            disp([k,irepeat]);
%        end
%        grzQ=grzQ+tau*grzd;
%        grzQ0=grzQ0+tau+grzd0;
%        disp(gr1);
        if gr1>-eps_z
%            disp([e1'*ga,e1'*gb,irepeat]);
            break;
        end

        
% diagnosis
        ff=zz.z'*(0.5*grzQ0+s0);
        fff=zz.z'*(0.5*grzQ+s);
        
%        ffx(irepeat,k)=ff;
%        fffx(irepeat,k)=fff;
        
        if fff<fffmin
%            fffmin=fff;
        end
        if ff<ffmin
            ffmin=ff;
        end
        
        
        
        
    end % irepeat
   
        ck0=ck;    
        ga=Y.*zz.z(1:m)-zz.z(2*m+1:3*m);
        gb=Y.*zz.z(m+1:2*m)+zz.z(2*m+1:3*m);
    
% update of the Lagrangian    
        h(1)=e1'*ga;
        h(2)=e1'*gb;

        lambda0=lambda;
        lambda=lambda+ck*h/1;
% here we can make more sophisticated update of the running penalty parameter    
        lerr=sum((lambda-lambda0).^2);
        herr0=herr;
        herr=sum(h.^2);
%        if herr0<realmax
%            cgrow=(herr0/herr)^2;
%        end
        format('short','g')
        %disp([herr,lambda(1),lambda(2),ck*h(1),ck*h(2),gr1]);
        ck=cgrow*ck;
%        disp(abs(herr-herr0));
        format('short');
        if (herr<eps_lambda) || abs((herr/herr0)-1)<0.01   
%        if (herr<eps_lambda) || abs((herr/herr0)-1)<0.005
%        if abs(herr-herr0)<0.2;
%        if abs((herr0/herr)-1)<0.02;
%        if herr>herr0
%            zz.z=zz0;
            break;
        end
        
%    disp([k,ck,cgrow]);
    
%    disp('--------');
%    disp([lerr,herr]);
%    disp(lambda');
    xlerr(k)=lerr;
    xherr(k)=herr;
    xckerr(k)=ck;
    
    herr0=herr;
    
    
    
end % k

%zz.z=zmin;

% disp([fffmin,ffmin]);

%disp(lambda);
alpha_A=zz.z(1:m);
alpha_B=zz.z(m+1:2*m);
u_N=zz.z(2*m+1:3*m);
