% Created by Dakota Flanary
clear;
close all;

addpath(genpath('Functions'))


load('Data/GraphPwelchedData/test-point-F_graph-pwelched-data.mat'); % loads in the signal data
foxtrot = graphPwelchedData;
load('Data/GraphPwelchedData/test-point-G_graph-pwelched-data.mat'); % loads in the signal data
golf = graphPwelchedData;
load('Data/GraphPwelchedData/test-point-H_graph-pwelched-data.mat'); % loads in the signal data
hotel = graphPwelchedData;
load('Data/GraphPwelchedData/test-point-I_graph-pwelched-data.mat'); % loads in the signal data
india = graphPwelchedData;
load('Data/GraphPwelchedData/test-point-J_graph-pwelched-data.mat'); % loads in the signal data
juliet = graphPwelchedData;
clear graphPwelchedData;
fox_max = max(max(foxtrot));
golf_max = max(max(golf));
hotel_max = max(max(hotel));
india_max = max(max(india));
juliet_max = max(max(juliet));
normalize_max = sqrt(max([fox_max,golf_max,hotel_max,india_max,juliet_max]));
foxtrot = (sqrt(foxtrot)/normalize_max).^2;
golf = (sqrt(golf)/normalize_max).^2;
hotel = (sqrt(hotel)/normalize_max).^2;
india = (sqrt(india)/normalize_max).^2;
juliet = (sqrt(juliet)/normalize_max).^2;

num_loops = 20; % number of data points to test
numLocations= 2290;


foxtrot_cap = zeros(numLocations,num_loops); % vector for harrison's capacity
golf_cap = zeros(numLocations,num_loops); % vector for sma
hotel_cap = zeros(numLocations,num_loops);
india_cap = zeros(numLocations,num_loops);
juliet_cap = zeros(numLocations,num_loops);
golf_sec_cap = zeros(numLocations,num_loops); % vector for sma
hotel_sec_cap = zeros(numLocations,num_loops);
india_sec_cap = zeros(numLocations,num_loops);
juliet_sec_cap = zeros(numLocations,num_loops);
snr = logspace(-3,5,num_loops); % snr values to test(in linear)
tic;
for index = 1:num_loops
    %% Capacity
    [foxtrot_cap(:,index), foxtrot_carrier_capacity] = gaussian_capacity(foxtrot,snr(index));
    [golf_cap(:,index), golf_carrier_capacity] = gaussian_capacity(golf,snr(index));
    [hotel_cap(:,index), hotel_carrier_capacity] = gaussian_capacity(hotel,snr(index));
    [india_cap(:,index), india_carrier_capacity] = gaussian_capacity(india,snr(index));
    [juliet_cap(:,index), juliet_carrier_capacity] = gaussian_capacity(juliet,snr(index));
 
    %% Secrecy Capacity
    for x = 1 : numLocations
        for carrier = 1:32
            g_sec = foxtrot_carrier_capacity(x,carrier) - ...
                golf_carrier_capacity(x,carrier);
            if g_sec < 0
                g_sec = 0;
            end
            golf_sec_cap(x,index) = golf_sec_cap(x,index) + g_sec;
            
            h_sec = foxtrot_carrier_capacity(x,carrier) - ...
                hotel_carrier_capacity(x,carrier);
            if h_sec < 0
                h_sec = 0;
            end
            hotel_sec_cap(x,index) = hotel_sec_cap(x,index) + h_sec;
            
            i_sec = foxtrot_carrier_capacity(x,carrier) - ...
                india_carrier_capacity(x,carrier);
            if i_sec < 0
                i_sec = 0;
            end
            india_sec_cap(x,index) = india_sec_cap(x,index) + i_sec;
            
            j_sec = foxtrot_carrier_capacity(x,carrier) - ...
                juliet_carrier_capacity(x,carrier);
            if j_sec < 0
                j_sec = 0;
            end
            juliet_sec_cap(x,index) = juliet_sec_cap(x,index) + j_sec;
        end
    end
end
toc;

snr = 10*log10(snr);
nmax = 200;
for n = 1:nmax
   b_k(n) = 1/nmax; 
end
%%
for i = 1:num_loops
%     figure()
%     hold on;
%     plot(foxtrot_cap(:,i),'b');
%     plot(golf_cap(:,i),'g');
%     plot(hotel_cap(:,i),'y');
%     plot(india_cap(:,i),'r');
%     plot(juliet_cap(:,i),'k');
    figure()
    hold on
    plot(golf_sec_cap(:,i));
    plot(hotel_sec_cap(:,i));
    plot(india_sec_cap(:,i));
    plot(juliet_sec_cap(:,i));
    ylabel('Bits per channel use')
    xlabel('location');
    legend('Golf Sec Cap','Hotel Sec Cap','India Sec Cap','Juliet Sec Cap');
    hold off
    
    sgolf_sec_cap(:,i) = conv(golf_sec_cap(:,i),b_k);
    shotel_sec_cap(:,i) = conv(hotel_sec_cap(:,i),b_k);
    sindia_sec_cap(:,i) = conv(india_sec_cap(:,i),b_k);
    sjuliet_sec_cap(:,i) = conv(juliet_sec_cap(:,i),b_k);
    
    figure()
    hold on
    plot(sgolf_sec_cap(:,i));
    plot(shotel_sec_cap(:,i));
    plot(sindia_sec_cap(:,i));
    plot(sjuliet_sec_cap(:,i));
    ylabel('Bits per channel use')
    xlabel('location');
    legend('Golf Sec Cap','Hotel Sec Cap','India Sec Cap','Juliet Sec Cap');
    hold off
    
end






