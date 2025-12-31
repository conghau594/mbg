// #include <boost/asio.hpp>
// #include <boost/fiber/all.hpp>
// #include <boost/json.hpp>
// #include <iostream>
// #include <chrono>

// using namespace std::chrono_literals;
// namespace fibers = boost::fibers;
// namespace asio = boost::asio;

// void fiber_timer(asio::io_context &io, long long duration)
// {
//     asio::steady_timer timer(io);
//     timer.expires_after(std::chrono::seconds(duration));

//     std::cout << "Fiber " << boost::this_fiber::get_id()
//               << " in thread " << std::this_thread::get_id()
//               << ": waiting timer " << duration << "s...\n";

//     // Sử dụng future để đồng bộ fiber với Asio
//     boost::fibers::future<boost::system::error_code> fut =
//         boost::fibers::async([&timer]()
//                              {
//             boost::system::error_code ec;
//             timer.wait(ec);
//             return ec; });

//     auto ec = fut.get(); // fiber yield tới đây
//     if (!ec)
//         std::cout << "Fiber " << boost::this_fiber::get_id()
//                   << ": timer expired.\n";
//     else
//         std::cout << "Fiber " << boost::this_fiber::get_id()
//                   << ": timer error: " << ec.message() << "\n";
// }

// int main()
// {
//     std::cout << "Main thread id: " << std::this_thread::get_id() << "\n";

//     boost::json::value jv = {{"message", "Hi from Boost JSON!"}};
//     std::cout << boost::json::serialize(jv) << std::endl;

//     asio::io_context io;

//     // Tạo 2 fiber chạy công việc
//     fibers::fiber f1([&io]()
//                      { fiber_timer(io, 2); });

//     fibers::fiber f2([&io]()
//                      { fiber_timer(io, 10); });

//     // Chạy io_context trong fiber riêng
//     fibers::fiber io_fiber(
//         [&io]()
//         {
//             std::cout << "Fiber " << boost::this_fiber::get_id()
//                       << " in thread " << std::this_thread::get_id()
//                       << ": io running...\n";
//             io.run();
//         });

//     f1.detach();
//     f2.detach();
//     io.stop();
//     io_fiber.join();

//     std::cout << "Done.\n";
//     return 0;
// }

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>

using boost::asio::steady_timer;
using namespace std::chrono_literals;

void func(
    boost::asio::io_context &io,
    boost::asio::yield_context yield,
    long long duration, std::string id)
{
    boost::system::error_code ec;

    //
    steady_timer t1(io);
    t1.expires_after(std::chrono::seconds(1));
    t1.async_wait(yield[ec]); // chờ với yield_context
    if (!ec)
    {
        std::cout << "Timer 1 of " << id
                  << " in thread " << std::this_thread::get_id()
                  << " done\n";
    }
    else
    {
        std::cout << "Timer 1 of " << id
                  << " in thread " << std::this_thread::get_id()
                  << " error: " << ec.message() << "\n";
    }

    //
    steady_timer t2(io);
    t2.expires_after(std::chrono::seconds(duration));
    t2.async_wait(yield[ec]); // chờ với yield_context
    if (!ec)
    {
        std::cout << "Timer 2 of " << id
                  << " in thread " << std::this_thread::get_id()
                  << " done\n";
    }
    else
    {
        std::cout << "Timer 2 of " << id
                  << " in thread " << std::this_thread::get_id()
                  << " error: " << ec.message() << "\n";
    }
}

int main()
{
    boost::asio::io_context io;
    boost::asio::spawn(
        io,
        [&](boost::asio::yield_context yield)
        {
            func(io, yield, 2, "ID01");
        },
        boost::asio::detached);
    boost::asio::spawn(
        io,
        [&](boost::asio::yield_context yield)
        { func(io, yield, 5, "ID02"); },
        boost::asio::detached);

    std::thread io_thread1([&io]()
                           { io.run(); });
    std::thread io_thread2([&io]()
                           { io.run(); });
    io_thread1.join();
    io_thread2.join();
    return 0;
}
