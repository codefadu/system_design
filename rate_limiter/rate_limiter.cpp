#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>

using namespace std;



    //                      RateLimiter
    //                           |
    //          ┌────────────────┼────────────────┐
    //          ↓                ↓                ↓
    //   Config Manager       Factory         Default Config
    //          |                |
    //          |                ↓
    //          |       RateLimitingAlgorithm
    //          |          /       |       \
    //          |         /        |        \
    //          ↓        ↓         ↓         ↓
    //    RateLimiter  Fixed    Token     Sliding
    //     Config      Window   Bucket    Window
    //          |
    //          ↓
    //       endpoint



//                     ┌──────────────────┐
//                     │   RateLimiter    │
//                     └────────┬─────────┘
//                              │
//                        uses config
//                              ↓
//                     ┌──────────────────┐
//                     │ RateLimitConfig  │
//                     └────────┬─────────┘
//                              │
//                     tells algorithm
//                              ↓
//                     ┌──────────────────┐
//                     │    Factory       │
//                     └────────┬─────────┘
//                              │ creates
//                              ↓
//               ┌─────────────────────────────┐
//               │ RateLimitingAlgorithm       │
//               └──────────────┬──────────────┘
//                      ┌───────┼────────┐
//                      ↓       ↓        ↓
//                   Fixed   Token    Sliding
//                   Window  Bucket    Window


// RateLimiter
//      │
//      └──────────────→ RateLimitResult



            // main()
            //   |
            //   | config
            //   ↓
            // RateLimiter
            //   |
            //   | addConfig("/login", config)
            //   ↓
            // Factory::create(config)
            //   |
            //   | algorithm == "FixedWindow"
            //   ↓
            // FixedWindowRateLimiter
            //   |
            //   | check(clientId)
            //   ↓
            // RateLimitResult
            //   |
            //   ↓
            // main()

// ---------------- RESULT ----------------

struct RateLimitResult {
    bool allowed;
    int remaining;
    long retryAfterMs;
};


// ---------------- CONFIG ----------------

struct RateLimiterConfig {
    string algorithm;

    // Fixed Window
    int limit;
    int windowSeconds;

    // Token Bucket
    int capacity;
    double refillRate;
};


// ---------------- ALGORITHM INTERFACE ----------------

class RateLimitingAlgorithm {
public:
    virtual RateLimitResult check(string clientId) = 0;

    virtual ~RateLimitingAlgorithm() {}
};


// ---------------- FIXED WINDOW ----------------

class FixedWindowRateLimiter : public RateLimitingAlgorithm {

private:
    int limit;
    int windowSeconds;

    struct ClientData {
        int count;
        long windowStart;
    };

    unordered_map<string, ClientData> clients;

public:

    FixedWindowRateLimiter(int limit, int windowSeconds) {
        this->limit = limit;
        this->windowSeconds = windowSeconds;
    }

    RateLimitResult check(string clientId) override {

        long now = time(nullptr);

        if (!clients.count(clientId)) {
            clients[clientId] = {0, now};
        }

        ClientData &client = clients[clientId];

        // New window
        if (now - client.windowStart >= windowSeconds) {
            client.count = 0;
            client.windowStart = now;
        }

        // Limit reached
        if (client.count >= limit) {

            long retryAfter =
                (windowSeconds -
                 (now - client.windowStart)) * 1000;

            return {
                false,
                0,
                retryAfter
            };
        }

        client.count++;

        return {
            true,
            limit - client.count,
            0
        };
    }
};


// ---------------- OTHER ALGORITHMS ----------------

class TokenBucket : public RateLimitingAlgorithm {

public:

    RateLimitResult check(string clientId) override {
        // Implementation later
        return {true, 0, 0};
    }
};


class SlidingWindowLog : public RateLimitingAlgorithm {

public:

    RateLimitResult check(string clientId) override {
        // Implementation later
        return {true, 0, 0};
    }
};


// ---------------- FACTORY ----------------

class RateLimiterFactory {

public:

    static RateLimitingAlgorithm* create(
        const RateLimiterConfig& config
    ) {

        if (config.algorithm == "FixedWindow") {

            return new FixedWindowRateLimiter(
                config.limit,
                config.windowSeconds
            );
        }

        if (config.algorithm == "TokenBucket") {

            return new TokenBucket();
        }

        if (config.algorithm == "SlidingWindowLog") {

            return new SlidingWindowLog();
        }

        return nullptr;
    }
};


// ---------------- RATE LIMITER ----------------

class RateLimiter {

private:

    unordered_map<string, RateLimiterConfig> configs;

    unordered_map<string, RateLimitingAlgorithm*> algorithms;

public:

    void addConfig(
        string endpoint,
        RateLimiterConfig config
    ) {

        configs[endpoint] = config;

        algorithms[endpoint] =
            RateLimiterFactory::create(config);
    }


    RateLimitResult checkLimit(
        string clientId,
        string endpoint
    ) {

        // No configuration → default
        if (!configs.count(endpoint)) {
            return {true, 0, 0};
        }

        return algorithms[endpoint]->check(clientId);
    }
};


// ---------------- MAIN ----------------

int main() {

    RateLimiter limiter;


    // /login → Fixed Window
    RateLimiterConfig loginConfig{
        "FixedWindow",
        3,      // limit
        60,     // window seconds
        0,
        0
    };

    limiter.addConfig("/login", loginConfig);


    // Requests
    auto result1 =
        limiter.checkLimit("user1", "/login");

    cout << "Allowed: "
         << result1.allowed
         << ", Remaining: "
         << result1.remaining
         << endl;


    auto result2 =
        limiter.checkLimit("user1", "/login");

    cout << "Allowed: "
         << result2.allowed
         << ", Remaining: "
         << result2.remaining
         << endl;


    auto result3 =
        limiter.checkLimit("user1", "/login");

    cout << "Allowed: "
         << result3.allowed
         << ", Remaining: "
         << result3.remaining
         << endl;


    auto result4 =
        limiter.checkLimit("user1", "/login");

    cout << "Allowed: "
         << result4.allowed
         << ", Remaining: "
         << result4.remaining
         << ", Retry After: "
         << result4.retryAfterMs
         << " ms"
         << endl;


    return 0;
}