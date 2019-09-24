%%% mdrMovie

Fs = 100;
F0 = 0.3;
N = 3/F0;
L = 100;

stride = 1000*N; % numsamples needs to be multiple of 100 so the indexing 
                 % of the complex exponential all works
myalpha = 0.95;

%h = firls(2*L,[0 0.1 0.15 0.5]*2,[1 1 0 0]);

FF = (-0.5:1/N:0.5-1/N)'*Fs;

% Lindsey's data:

%filename = '/media/V2V/2019-02-21_data-collection/test-point-E-Lindsey_2019-02-21__13-55-39.mat';
%filename = '/media/V2V/2019-02-21_data-collection/test-point-D-Lindsey_2019-02-21__13-47-21.mat';
%filename = '/media/V2V/2019-02-21_data-collection/test-point-C-Lindsey_2019-02-21__13-39-03.mat';

m = matfile(filename);

% frequency shift and filter first NN samples to 
% see what we get.

h = firls(2*L,[0 0.02 0.05 0.5]*2,[1 1 0 0]);

NN = 200000;
N = 100;

temp = m.data(1:NN,1);
x = (temp - 8192)/8192;
y = temp.*cos(2*pi*0.29*(0:NN-1)');
ytemp = conv(h,y);
y2 = ytemp(L+1:end-L);

Py = pwelch(y2,boxcar(1000),0,1000,'twosided');
figure(90);
plot(-0.5:1/1000:0.5-1/1000,10*log10(fftshift(Py))); grid on

% return

figure(99);
plot(0:N-1,y2(1:N)); grid on;
ax = gca;
%ax.YLim = 40*[-1 1];

for idx = 2:NN/N-N
    ax.Children.YData = y2((idx-1)*N+1:idx*N);
    drawnow;
end







return

%%% get the first N to create the plot, define the axes, 
%%% and set the plot parameters

temp = m.data(1:N,1);
x = (temp - 8192)/8192;
XX = fft(x,N);

figure(10);
plot(FF,20*log10(fftshift(abs(XX))));
grid on;
ax = gca;

figure(11);
plot(x);
grid on;
ax2 = gca;
ax2.YLim = 20e-3 * [-1 1];
% ax2.YLim = 1 * [-1 1];

ax.XLim = 20*[-1 1];
ax.YLim = [-60 60];
ax.XLabel.String = 'frequency';
ax.YLabel.String = 'magnitude (dB)';
% ax.Title.String = sprintf('index = %4d',0);
ax.Children.Color = 'k';
ax.Children.LineWidth = 1;


% loop through the data numSamples at a time and update the plot

n1 = N;
for idx = 1:10720
    
    % pause(0.5);
    
    midx = (n1+1):(n1+N);
    temp = m.data(midx,1);
    
    x = (temp - 8192)/8192;
    ax2.Children.YData = x;
    
    XX = fft(x,N);
    ax.Children.YData = 20*log10(fftshift(abs(XX)));
    
    drawnow;
    
    n1 = n1 + stride;
end
