function [p_e] = probability_erasure(sigmaSquared,G,epsilon)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
b = [1 -1];
% tic;
% sigmaSquared = 16;
% G = 1;
% epsilon = .01;
%% Functions

fun_x = @(x) ((1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*(x-(G*b(1))).^2)+...
            (1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*(x-(G*b(2))).^2))...
            .*.5;
        
p_b = @(x,n) (1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*abs((x-(abs(G)*b(n)))).^2)...
    ./(2*fun_x(x));

H = @(x) -p_b(x,1).*log2(p_b(x,1)) - p_b(x,2).*log2(p_b(x,2));

I = @(x) 1-H(x);

e_1 = @(x) (I(x) < epsilon);

p_e1 = @(x) e_1(x) .* fun_x(x);
% q = integral(fun_x,-inf,inf)
p_e = integral(p_e1,-inf,inf);
% toc;
% end

%% graphs
% 
% figure()
% fplot(fun_x);
% xlim([-20*G,20*G]);
% figure()
% fplot(p_e1);
% xlim([-10*G,10*G]);
% figure();
% fplot(e_1);
% xlim([-10*G,10*G]);
% figure();
% fplot(H);
% xlim([-50*G,50*G]);
% figure();
% fplot(I);
% xlim([-50*G,50*G]);
end