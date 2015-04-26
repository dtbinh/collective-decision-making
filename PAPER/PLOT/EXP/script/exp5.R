A <- matrix(c(10, 0, 2, 18,
              5, 10, 1, 14,
              8, 18, 1, 3,
              9, 6, 8, 7,
              13, 3, 3, 11),
            nrow=4, ncol=5)

pdf("exp5.pdf", width=16/2.54, height=10/2.54,
    pointsize=10)
par(mgp=c(2.2,0.45,0), tcl=-0.4, mar=c(3.3,3.6,1.1,1.1))

barplot(A,
        names.arg=c(1, 2, 3, 4, 5),
        xlab="Run Count",
        ylab="Robot Count",
        col=c("magenta", "blue", "orange", "red"),
        beside=TRUE,
        ylim=c(0,30)
        )
legend(x=20.5, y=30,
       c("room 0", "room 1", "room 2", "room 3"),
       cex=1,
       title="Robot's opinion",
       col=c("magenta", "blue", "orange", "red"),
       pch=c(22,0,0)
       )
dev.off()

