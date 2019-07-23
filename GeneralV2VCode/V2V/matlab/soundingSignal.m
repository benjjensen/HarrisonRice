%%% soundingSignal

df = 0.3125; % subcarrier separation in MHz
BW = 20; % BW in MHz
Fs = 100; % sample rate in MHz
f0 = 70; % bandpass frequency in MHz

dF = df/Fs;
N0 = 1/dF;
F0 = f0/Fs;

% bulkphase = 3*pi/4;
bulkphase = 0;

n = (0:N0-1)';
xl = zeros(N0,1);
for idx = -31:32
    xl = xl + exp(1i*2*pi*idx*dF*n);
end
xl2 = repmat(xl,1000,1);
x = real(xl2.*exp(1i*2*pi*F0*(0:length(xl2)-1)')*exp(1i*bulkphase));

figure(1);
subplot(311); plot(n,real(xl)); grid on;
subplot(312); plot(n,abs(xl)); grid on;
subplot(313); plot(n,x(1:length(n))); grid on;

% figure(2); plot(x); grid on;

NN = 100;

figure(100);
plot(x(1:N0)); grid on; ax = gca;
title('idx = 1');

for idx = 2:length(x)/(NN*N0)-1
   ax.Children.YData = x((idx-1)*N0+1:idx*N0);
   ax.Title.String = sprintf('idx = %d\n',idx);
   pause(1);
end