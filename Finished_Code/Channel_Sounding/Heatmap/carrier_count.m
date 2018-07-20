
close all;

load('camacho.mat');
load('chambers.mat');
load('conference.mat');
load('hall_pwelch.mat');
load('harrison.mat');
load('smalley.mat');
load('tx2camacho.mat');
load('tx2chambers.mat');
load('tx2conference.mat');
load('tx2hallway.mat');
load('tx2harrison.mat');
load('tx2smalley.mat');

threshold = 10;
tx2threshold = 25;

cam1 = find_num_carriers(camacho,threshold);
cam2 = find_num_carriers(tx2camacho,tx2threshold);
chambers1 = find_num_carriers(chambers,threshold);
chambers2 = find_num_carriers(tx2chambers,tx2threshold);
conference1 = find_num_carriers(conference,threshold);
conference2 = find_num_carriers(tx2conference,tx2threshold);
harrison1 = find_num_carriers(harrison,threshold);
harrison2 = find_num_carriers(tx2harrison,tx2threshold);
smalley1 = find_num_carriers(smalley,threshold);
smalley2 = find_num_carriers(tx2smalley,tx2threshold);

figure(2);
histogram(cam2)
title('Camacho');
figure(1);
histogram(harrison2)
title('Harrison');
figure(3);
histogram(conference2)
title('Conference');
figure(4);
histogram(smalley2)
title('Smalley');

camacho_best_carrier = find_best_carriers(cam1,camacho,3,threshold);
camacho_best_carrier2 = find_best_carriers(cam2,tx2camacho,3,tx2threshold);
chambers_best_carrier = find_best_carriers(chambers1,chambers,2,threshold);
chambers_best_carrier2 = find_best_carriers(chambers2,tx2chambers,2,tx2threshold);
conference_best_carrier = find_best_carriers(conference1,conference,1,threshold);
conference_best_carrier2 = find_best_carriers(conference2,tx2conference,1,tx2threshold);
harrison_best_carrier = find_best_carriers(harrison1,harrison,5,threshold);
harrison_best_carrier2 = find_best_carriers(harrison2,tx2harrison,5,tx2threshold);
smalley_best_carrier = find_best_carriers(smalley1,smalley,4,threshold);
smalley_best_carrier2 = find_best_carriers(smalley2,tx2smalley,4,tx2threshold);

figure(5);
hold on;
title(['\fontsize{12} Tx_2 @' num2str(tx2threshold) ' dB']);
bar(harrison_best_carrier2,'DisplayName',['Harrison - ' num2str(sum(harrison_best_carrier2)/5)]);
bar(smalley_best_carrier2,'DisplayName',['Smalley - ' num2str(sum(smalley_best_carrier2)/4)]);
bar(chambers_best_carrier2,'DisplayName',['Chambers - ' num2str(sum(chambers_best_carrier2)/2)]);
bar(camacho_best_carrier2,'DisplayName',['Camacho - ' num2str(sum(camacho_best_carrier2)/3)]);
bar(conference_best_carrier2,'DisplayName',['Conference - ' num2str(sum(conference_best_carrier2))]);
legend;
hold off
