%All the points of sight in seconds driving from sundance to the highway:)
X = [527 552 552 566 566 568.3 568.3 571 571 575 575 581 581 584.8 584.8 596 596 603 603 611 611 629 629 656 656 661 661 680 680 682 682 722 722 732 732 771.5 771.5 805 805];
X1 = X - 527;
Y1 = [1 1  0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0 1 1 0];


figure()
plot(X1, Y1, 'LineWidth',2);
ylim([0 1.5]);
xlim([0 300]);
ylabel('Line of Sight');
xlabel('Time Elapsed (s)');
