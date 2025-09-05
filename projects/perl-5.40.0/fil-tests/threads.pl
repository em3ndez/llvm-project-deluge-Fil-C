#!/usr/bin/env perl

use strict;
use warnings;
use threads;
use threads::shared;
use Time::HiRes qw(sleep);

# Shared variables for thread communication
my $counter :shared = 0;
my $done :shared = 0;
my @results :shared;

# Simple thread function
sub worker_thread {
    my ($id, $iterations) = @_;
    print "Thread $id starting\n";
    
    for (my $i = 0; $i < $iterations; $i++) {
        # Increment shared counter
        {
            lock($counter);
            $counter++;
        }
        
        # Simulate some work
        sleep(0.1);
        
        # Store result
        {
            lock(@results);
            push @results, "Thread $id: iteration $i";
        }
    }
    
    print "Thread $id finishing\n";
    return "Thread $id completed $iterations iterations";
}

# Thread that demonstrates detached threads
sub background_worker {
    my $id = shift;
    print "Background thread $id started (detached)\n";
    sleep(2);
    print "Background thread $id finished\n";
}

# Main program
print "=== Perl Threads Example ===\n\n";

# Create and join threads
print "Creating joinable threads...\n";
my @threads;
for my $i (1..3) {
    push @threads, threads->create(\&worker_thread, $i, 3);
}

# Create detached threads
print "\nCreating detached threads...\n";
for my $i (1..2) {
    my $thr = threads->create(\&background_worker, $i);
    $thr->detach();
}

# Wait for joinable threads
print "\nWaiting for joinable threads...\n";
my @thread_results;
foreach my $thr (@threads) {
    push @thread_results, $thr->join();
}

# Display results
print "\n=== Results ===\n";
print "Thread return values:\n";
foreach my $result (@thread_results) {
    print "  $result\n";
}

print "\nShared counter value: $counter\n";
print "Number of results stored: " . scalar(@results) . "\n";

# Thread information
print "\n=== Thread Information ===\n";
print "Main thread ID: " . threads->tid() . "\n";
print "Number of threads created: " . threads->list(threads::all) . "\n";
print "Number of running threads: " . threads->list(threads::running) . "\n";

# Wait a bit for detached threads to finish
print "\nWaiting for detached threads to complete...\n";
sleep(3);

# Thread stack size example
print "\n=== Thread Stack Size ===\n";
print "Default stack size: " . threads->get_stack_size() . " bytes\n";
threads->set_stack_size(64*1024);  # 64KB
print "New stack size: " . threads->get_stack_size() . " bytes\n";

# Thread with error handling
sub error_prone_thread {
    my $should_die = shift;
    if ($should_die) {
        die "Intentional error in thread!";
    }
    return "Success";
}

print "\n=== Thread Error Handling ===\n";
my $error_thread = threads->create(\&error_prone_thread, 1);
eval {
    $error_thread->join();
};
if ($@) {
    print "Caught thread error: $@\n";
}

# Thread yielding example
sub yielding_thread {
    my $id = shift;
    for (1..3) {
        print "Thread $id working...\n";
        threads->yield();  # Give other threads a chance to run
        sleep(0.1);
    }
}

print "\n=== Thread Yielding ===\n";
my @yield_threads = map { threads->create(\&yielding_thread, $_) } (1..2);
$_->join() for @yield_threads;

print "\n=== Program Complete ===\n";