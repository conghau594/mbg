# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(bggclient LANGUAGES CXX)

message(STATUS "############################################################")
message(STATUS "CMakeLists.txt for 'bggclient' in ${CMAKE_CURRENT_SOURCE_DIR}")



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE CACHE BOOL "")
set(BOOST_SUPERPROJECT_VERSION "1.88.0")

include(FetchContent)

###############################################################################
# FetchContent for SFML                                                       #
###############################################################################
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
  )
  FetchContent_MakeAvailable(SFML)
  FetchContent_GetProperties(SFML POPULATED SFML_POPULATED)
  message(STATUS "+ SFML populated: ${SFML_POPULATED}")

###############################################################################
# FetchContent for ImGui                                                      #
###############################################################################
FetchContent_Declare(ImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.91.1
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

FetchContent_GetProperties(ImGui POPULATED ImGui_POPULATED)
message(STATUS "+ ImGui populated: ${ImGui_POPULATED}")

###############################################################################
# FetchContent for spdlog                                                      #
###############################################################################
FetchContent_Declare(SpdLog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG v1.15.3
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(SpdLog)
FetchContent_GetProperties(SpdLog POPULATED SpdLog_POPULATED)
message(STATUS "+ SpdLog populated: ${SpdLog_POPULATED}")

###############################################################################
# FetchContent for ImGui-SFML                                                 #
###############################################################################
FetchContent_Declare(ImGuiSfml
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml
  GIT_TAG v3.0
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(ImGuiSfml)
FetchContent_GetProperties(ImGuiSfml POPULATED ImGuiSfml_POPULATED)
message(STATUS "+ ImGuiSfml populated: ${ImGuiSfml_POPULATED}")

###############################################################################
# First: Fetch Boost::config                                                   #
###############################################################################
FetchContent_Declare(BoostConfig
  GIT_REPOSITORY https://github.com/boostorg/config.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostConfig)
FetchContent_GetProperties(BoostConfig POPULATED BoostConfig_POPULATED)
message(STATUS "+ BoostConfig populated: ${BoostConfig_POPULATED}")

###############################################################################
# Then: Fetch Boost::assert                                                    #
###############################################################################
FetchContent_Declare(BoostAssert
  GIT_REPOSITORY https://github.com/boostorg/assert.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostAssert)
FetchContent_GetProperties(BoostAssert POPULATED BoostAssert_POPULATED)
message(STATUS "+ BoostAssert populated: ${BoostAssert_POPULATED}")

###############################################################################
# Fetch Boost::type_index                                                       #
###############################################################################
FetchContent_Declare(BoostTypeIndex
  GIT_REPOSITORY https://github.com/boostorg/type_index.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTypeIndex)
FetchContent_GetProperties(BoostTypeIndex POPULATED BoostTypeIndex_POPULATED)
message(STATUS "+ BoostTypeIndex populated: ${BoostTypeIndex_POPULATED}")

###############################################################################
# Fetch Boost::pool                                                       #
###############################################################################
FetchContent_Declare(BoostPool
  GIT_REPOSITORY https://github.com/boostorg/pool.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostPool)
FetchContent_GetProperties(BoostPool POPULATED BoostPool_POPULATED)
message(STATUS "+ BoostPool populated: ${BoostPool_POPULATED}")

###############################################################################
# Fetch Boost::date_time                                                       #
###############################################################################
FetchContent_Declare(BoostDateTime
  GIT_REPOSITORY https://github.com/boostorg/date_time.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostDateTime)
FetchContent_GetProperties(BoostDateTime POPULATED BoostDateTime_POPULATED)
message(STATUS "+ BoostDateTime populated: ${BoostDateTime_POPULATED}")

###############################################################################
# Fetch Boost::winapi                                                       #
###############################################################################
FetchContent_Declare(BoostWinApi
  GIT_REPOSITORY https://github.com/boostorg/winapi.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostWinApi)
FetchContent_GetProperties(BoostWinApi POPULATED BoostWinApi_POPULATED)
message(STATUS "+ BoostWinApi populated: ${BoostWinApi_POPULATED}")

###############################################################################
# Fetch Boost::variant2                                                       #
###############################################################################
FetchContent_Declare(BoostVariant2
  GIT_REPOSITORY https://github.com/boostorg/variant2.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostVariant2)
FetchContent_GetProperties(BoostVariant2 POPULATED BoostVariant2_POPULATED)
message(STATUS "+ BoostVariant2 populated: ${BoostVariant2_POPULATED}")

###############################################################################
# Fetch Boost::integer                                                       #
###############################################################################
FetchContent_Declare(BoostInteger
  GIT_REPOSITORY https://github.com/boostorg/integer.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostInteger)
FetchContent_GetProperties(BoostInteger POPULATED BoostInteger_POPULATED)
message(STATUS "+ BoostInteger populated: ${BoostInteger_POPULATED}")

###############################################################################
# Fetch Boost::algorithm                                                       #
###############################################################################
FetchContent_Declare(BoostAlgorithm
  GIT_REPOSITORY https://github.com/boostorg/algorithm.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostAlgorithm)
FetchContent_GetProperties(BoostAlgorithm POPULATED BoostAlgorithm_POPULATED)
message(STATUS "+ BoostAlgorithm populated: ${BoostAlgorithm_POPULATED}")

###############################################################################
# Fetch Boost::predef                                                       #
###############################################################################
FetchContent_Declare(BoostPredef
  GIT_REPOSITORY https://github.com/boostorg/predef.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostPredef)
FetchContent_GetProperties(BoostPredef POPULATED BoostPredef_POPULATED)
message(STATUS "+ BoostPredef populated: ${BoostPredef_POPULATED}")

###############################################################################
# Fetch Boost::io                                                       #
###############################################################################
FetchContent_Declare(BoostIo
  GIT_REPOSITORY https://github.com/boostorg/io.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostIo)
FetchContent_GetProperties(BoostIo POPULATED BoostIo_POPULATED)
message(STATUS "+ BoostIo populated: ${BoostIo_POPULATED}")

###############################################################################
# Fetch Boost::array                                                       #
###############################################################################
FetchContent_Declare(BoostArray
  GIT_REPOSITORY https://github.com/boostorg/array.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostArray)
FetchContent_GetProperties(BoostArray POPULATED BoostArray_POPULATED)
message(STATUS "+ BoostArray populated: ${BoostArray_POPULATED}")

###############################################################################
# Fetch Boost::lexical_cast                                                       #
###############################################################################
FetchContent_Declare(BoostLexicalCast
  GIT_REPOSITORY https://github.com/boostorg/lexical_cast.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostLexicalCast)
FetchContent_GetProperties(BoostLexicalCast POPULATED BoostLexicalCast_POPULATED)
message(STATUS "+ BoostLexicalCast populated: ${BoostLexicalCast_POPULATED}")

###############################################################################
# Fetch Boost::concept_check                                                       #
###############################################################################
FetchContent_Declare(BoostConceptCheck
  GIT_REPOSITORY https://github.com/boostorg/concept_check.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostConceptCheck)
FetchContent_GetProperties(BoostConceptCheck POPULATED BoostConceptCheck_POPULATED)
message(STATUS "+ BoostConceptCheck populated: ${BoostConceptCheck_POPULATED}")

###############################################################################
# Fetch Boost::system                                                    #
###############################################################################
FetchContent_Declare(BoostSystem
  GIT_REPOSITORY https://github.com/boostorg/system.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostSystem)
FetchContent_GetProperties(BoostSystem POPULATED BoostSystem_POPULATED)
message(STATUS "+ BoostSystem populated: ${BoostSystem_POPULATED}")

###############################################################################
# Fetch Boost::numeric_conversion                                                    #
###############################################################################
FetchContent_Declare(BoostNumericConversion
  GIT_REPOSITORY https://github.com/boostorg/numeric_conversion.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostNumericConversion)
FetchContent_GetProperties(BoostNumericConversion POPULATED BoostNumericConversion_POPULATED)
message(STATUS "+ BoostNumericConversion populated: ${BoostNumericConversion_POPULATED}")

###############################################################################
# Fetch Boost::tuple                                                    #
###############################################################################
FetchContent_Declare(BoostTuple
  GIT_REPOSITORY https://github.com/boostorg/tuple.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTuple)
FetchContent_GetProperties(BoostTuple POPULATED BoostTuple_POPULATED)
message(STATUS "+ BoostTuple populated: ${BoostTuple_POPULATED}")

###############################################################################
# Fetch Boost::function                                                    #
###############################################################################
FetchContent_Declare(BoostFunction
  GIT_REPOSITORY https://github.com/boostorg/function.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostFunction)
FetchContent_GetProperties(BoostFunction POPULATED BoostFunction_POPULATED)
message(STATUS "+ BoostFunction populated: ${BoostFunction_POPULATED}")

###############################################################################
# Fetch Boost::range                                                    #
###############################################################################
FetchContent_Declare(BoostRange
  GIT_REPOSITORY https://github.com/boostorg/range.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostRange)
FetchContent_GetProperties(BoostRange POPULATED BoostRange_POPULATED)
message(STATUS "+ BoostRange populated: ${BoostRange_POPULATED}")

###############################################################################
# Fetch Boost::iterator                                                    #
###############################################################################
FetchContent_Declare(BoostIterator
  GIT_REPOSITORY https://github.com/boostorg/iterator.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostIterator)
FetchContent_GetProperties(BoostIterator POPULATED BoostIterator_POPULATED)
message(STATUS "+ BoostIterator populated: ${BoostIterator_POPULATED}")

###############################################################################
# Fetch Boost::tokenizer                                                    #
###############################################################################
FetchContent_Declare(BoostTokenizer
  GIT_REPOSITORY https://github.com/boostorg/tokenizer.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTokenizer)
FetchContent_GetProperties(BoostTokenizer POPULATED BoostTokenizer_POPULATED)
message(STATUS "+ BoostTokenizer populated: ${BoostTokenizer_POPULATED}")

###############################################################################
# Fetch Boost::mpl                                                    #
###############################################################################
FetchContent_Declare(BoostMpl
  GIT_REPOSITORY https://github.com/boostorg/mpl.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostMpl)
FetchContent_GetProperties(BoostMpl POPULATED BoostMpl_POPULATED)
message(STATUS "+ BoostMpl populated: ${BoostMpl_POPULATED}")

###############################################################################
# Fetch Boost::detail                                                    #
###############################################################################
FetchContent_Declare(BoostDetail
  GIT_REPOSITORY https://github.com/boostorg/detail.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostDetail)
FetchContent_GetProperties(BoostDetail POPULATED BoostDetail_POPULATED)
message(STATUS "+ BoostDetail populated: ${BoostDetail_POPULATED}")

###############################################################################
# Fetch Boost::conversion                                                    #
###############################################################################
FetchContent_Declare(BoostConversion
  GIT_REPOSITORY https://github.com/boostorg/conversion.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostConversion)
FetchContent_GetProperties(BoostConversion POPULATED BoostConversion_POPULATED)
message(STATUS "+ BoostConversion populated: ${BoostConversion_POPULATED}")

###############################################################################
# Fetch Boost::utility                                                    #
###############################################################################
FetchContent_Declare(BoostUtility
  GIT_REPOSITORY https://github.com/boostorg/utility.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostUtility)
FetchContent_GetProperties(BoostUtility POPULATED BoostUtility_POPULATED)
message(STATUS "+ BoostUtility populated: ${BoostUtility_POPULATED}")

###############################################################################
# Fetch Boost::regex                                                    #
###############################################################################
FetchContent_Declare(BoostRegex
  GIT_REPOSITORY https://github.com/boostorg/regex.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostRegex)
FetchContent_GetProperties(BoostRegex POPULATED BoostRegex_POPULATED)
message(STATUS "+ BoostRegex populated: ${BoostRegex_POPULATED}")

###############################################################################
# Fetch Boost::fusion                                                    #
###############################################################################
FetchContent_Declare(BoostFusion
  GIT_REPOSITORY https://github.com/boostorg/fusion.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostFusion)
FetchContent_GetProperties(BoostFusion POPULATED BoostFusion_POPULATED)
message(STATUS "+ BoostFusion populated: ${BoostFusion_POPULATED}")

###############################################################################
# Fetch Boost::unordered                                                    #
###############################################################################
FetchContent_Declare(BoostUnordered
  GIT_REPOSITORY https://github.com/boostorg/unordered.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostUnordered)
FetchContent_GetProperties(BoostUnordered POPULATED BoostUnordered_POPULATED)
message(STATUS "+ BoostUnordered populated: ${BoostUnordered_POPULATED}")

###############################################################################
# Fetch Boost::function_types                                                    #
###############################################################################
FetchContent_Declare(BoostFunctionTypes
  GIT_REPOSITORY https://github.com/boostorg/function_types.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostFunctionTypes)
FetchContent_GetProperties(BoostFunctionTypes POPULATED BoostFunctionTypes_POPULATED)
message(STATUS "+ BoostFunctionTypes populated: ${BoostFunctionTypes_POPULATED}")

###############################################################################
# Fetch Boost::exception                                                    #
###############################################################################
FetchContent_Declare(BoostException
  GIT_REPOSITORY https://github.com/boostorg/exception.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostException)
FetchContent_GetProperties(BoostException POPULATED BoostException_POPULATED)
message(STATUS "+ BoostException populated: ${BoostException_POPULATED}")

###############################################################################
# Fetch Boost::throw_exception                                                    #
###############################################################################
FetchContent_Declare(BoostThrowException
  GIT_REPOSITORY https://github.com/boostorg/throw_exception.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostThrowException)
FetchContent_GetProperties(BoostThrowException POPULATED BoostThrowException_POPULATED)
message(STATUS "+ BoostThrowException populated: ${BoostThrowException_POPULATED}")

###############################################################################
# Fetch Boost::type_traits                                                    #
###############################################################################
FetchContent_Declare(BoostTypeTraits
  GIT_REPOSITORY https://github.com/boostorg/type_traits.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTypeTraits)
FetchContent_GetProperties(BoostTypeTraits POPULATED BoostTypeTraits_POPULATED)
message(STATUS "+ BoostTypeTraits populated: ${BoostTypeTraits_POPULATED}")

###############################################################################
# Fetch Boost::static_assert                                                    #
###############################################################################
FetchContent_Declare(BoostStaticAssert
  GIT_REPOSITORY https://github.com/boostorg/static_assert.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostStaticAssert)
FetchContent_GetProperties(BoostStaticAssert POPULATED BoostStaticAssert_POPULATED)
message(STATUS "+ BoostStaticAssert populated: ${BoostStaticAssert_POPULATED}")

###############################################################################
# Then: Fetch Boost::uuid                                                      #
###############################################################################
FetchContent_Declare(BoostUuid
  GIT_REPOSITORY https://github.com/boostorg/uuid.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostUuid)
FetchContent_GetProperties(BoostUuid POPULATED BoostUuid_POPULATED)
message(STATUS "+ BoostUuid populated: ${BoostUuid_POPULATED}")
  
###############################################################################
# Fetch Boost::bind                                                      #
###############################################################################
FetchContent_Declare(BoostBind
  GIT_REPOSITORY https://github.com/boostorg/bind.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostBind)
FetchContent_GetProperties(BoostBind POPULATED BoostBind_POPULATED)
message(STATUS "+ BoostBind populated: ${BoostBind_POPULATED}")

###############################################################################
# Fetch Boost::logic                                                      #
###############################################################################
FetchContent_Declare(BoostLogic
  GIT_REPOSITORY https://github.com/boostorg/logic.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostLogic)
FetchContent_GetProperties(BoostLogic POPULATED BoostLogic_POPULATED)
message(STATUS "+ BoostLogic populated: ${BoostLogic_POPULATED}")
 
###############################################################################
# Fetch Boost::preprocessor                                                      #
###############################################################################
FetchContent_Declare(BoostPreprocessor
  GIT_REPOSITORY https://github.com/boostorg/preprocessor.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostPreprocessor)
FetchContent_GetProperties(BoostPreprocessor POPULATED BoostPreprocessor_POPULATED)
message(STATUS "+ BoostPreprocessor populated: ${BoostPreprocessor_POPULATED}")
 
###############################################################################
# Fetch Boost::optional                                                      #
###############################################################################
FetchContent_Declare(BoostOptional
  GIT_REPOSITORY https://github.com/boostorg/optional.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostOptional)
FetchContent_GetProperties(BoostOptional POPULATED BoostOptional_POPULATED)
message(STATUS "+ BoostOptional populated: ${BoostOptional_POPULATED}")
 

message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

###############################################################################
# Fetch Boost::beast                                                      #
###############################################################################
FetchContent_Declare(BoostBeast
  GIT_REPOSITORY https://github.com/boostorg/beast.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostBeast)
FetchContent_GetProperties(BoostBeast POPULATED BoostBeast_POPULATED)
message(STATUS "+ BoostBeast populated: ${BoostBeast_POPULATED}")

###############################################################################
# Fetch Boost::context                                                      #
###############################################################################
FetchContent_Declare(BoostContext
  GIT_REPOSITORY https://github.com/boostorg/context.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostContext)
FetchContent_GetProperties(BoostContext POPULATED BoostContext_POPULATED)
message(STATUS "+ BoostContext populated: ${BoostContext_POPULATED}")
 
###############################################################################
# Fetch Boost::asio                                                      #
###############################################################################
FetchContent_Declare(BoostAsio
  GIT_REPOSITORY https://github.com/boostorg/asio.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostAsio)
FetchContent_GetProperties(BoostAsio POPULATED BoostAsio_POPULATED)
message(STATUS "+ BoostAsio populated: ${BoostAsio_POPULATED}")

###############################################################################
# Fetch Boost::core                                                            #
###############################################################################
FetchContent_Declare(BoostCore
  GIT_REPOSITORY https://github.com/boostorg/core.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostCore)
FetchContent_GetProperties(BoostCore POPULATED BoostCore_POPULATED)
message(STATUS "+ BoostCore populated: ${BoostCore_POPULATED}")
  
###############################################################################
# Fetch Boost::move                                                      #
###############################################################################
FetchContent_Declare(BoostMove
  GIT_REPOSITORY https://github.com/boostorg/move.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostMove)
FetchContent_GetProperties(BoostMove POPULATED BoostMove_POPULATED)
message(STATUS "+ BoostMove populated: ${BoostMove_POPULATED}")

###############################################################################
# Fetch Boost::intrusive                                                      #
###############################################################################
FetchContent_Declare(BoostIntrusive
  GIT_REPOSITORY https://github.com/boostorg/intrusive.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostIntrusive)
FetchContent_GetProperties(BoostIntrusive POPULATED BoostIntrusive_POPULATED)
message(STATUS "+ BoostIntrusive populated: ${BoostIntrusive_POPULATED}")

###############################################################################
# Fetch Boost::endian                                                      #
###############################################################################
FetchContent_Declare(BoostEndian
  GIT_REPOSITORY https://github.com/boostorg/endian.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostEndian)
FetchContent_GetProperties(BoostEndian POPULATED BoostEndian_POPULATED)
message(STATUS "+ BoostEndian populated: ${BoostEndian_POPULATED}")

###############################################################################
# Fetch Boost::mp11                                                      #
###############################################################################
FetchContent_Declare(BoostMp11
  GIT_REPOSITORY https://github.com/boostorg/mp11.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostMp11)
FetchContent_GetProperties(BoostMp11 POPULATED BoostMp11_POPULATED)
message(STATUS "+ BoostMp11 populated: ${BoostMp11_POPULATED}")

###############################################################################
# Fetch Boost::smart_ptr                                                      #
###############################################################################
FetchContent_Declare(BoostSmartPtr
  GIT_REPOSITORY https://github.com/boostorg/smart_ptr.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostSmartPtr)
FetchContent_GetProperties(BoostSmartPtr POPULATED BoostSmartPtr_POPULATED)
message(STATUS "+ BoostSmartPtr populated: ${BoostSmartPtr_POPULATED}")

###############################################################################
# Fetch Boost::describe                                                      #
###############################################################################
FetchContent_Declare(BoostDescribe
  GIT_REPOSITORY https://github.com/boostorg/describe.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostDescribe)
FetchContent_GetProperties(BoostDescribe POPULATED BoostDescribe_POPULATED)
message(STATUS "+ BoostDescribe populated: ${BoostDescribe_POPULATED}")

###############################################################################
# Then: Fetch Boost::container_hash                                                      #
###############################################################################
FetchContent_Declare(BoostContainerHash
  GIT_REPOSITORY https://github.com/boostorg/container_hash.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostContainerHash)
FetchContent_GetProperties(BoostContainerHash POPULATED BoostContainerHash_POPULATED)
message(STATUS "+ BoostContainerHash populated: ${BoostContainerHash_POPULATED}")

###############################################################################
# Fetch Boost::container                                                      #
###############################################################################
FetchContent_Declare(BoostContainer
  GIT_REPOSITORY https://github.com/boostorg/container.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostContainer)
FetchContent_GetProperties(BoostContainer POPULATED BoostContainer_POPULATED)
message(STATUS "+ BoostContainer populated: ${BoostContainer_POPULATED}")

###############################################################################
# Then: Fetch Boost::align                                                      #
###############################################################################
FetchContent_Declare(BoostAlign
  GIT_REPOSITORY https://github.com/boostorg/align.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostAlign)
FetchContent_GetProperties(BoostAlign POPULATED BoostAlign_POPULATED)
message(STATUS "+ BoostAlign populated: ${BoostAlign_POPULATED}")

###############################################################################
# Then: Fetch Boost::json                                                      #
###############################################################################
FetchContent_Declare(BoostJson
  GIT_REPOSITORY https://github.com/boostorg/json.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostJson)
FetchContent_GetProperties(BoostJson POPULATED BoostJson_POPULATED)
message(STATUS "+ BoostJson populated: ${BoostJson_POPULATED}")

###############################################################################
# Fetch Boost::headers                                                      #
###############################################################################
FetchContent_Declare(BoostHeaders
  GIT_REPOSITORY https://github.com/boostorg/headers.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostHeaders)
FetchContent_GetProperties(BoostHeaders POPULATED BoostHeaders_POPULATED)
message(STATUS "+ BoostHeaders populated: ${BoostHeaders_POPULATED}")

###############################################################################
# Fetch Boost::static_string                                                  #
###############################################################################
FetchContent_Declare(BoostStaticString
  GIT_REPOSITORY https://github.com/boostorg/static_string.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostStaticString)
FetchContent_GetProperties(BoostStaticString POPULATED BoostStaticString_POPULATED)
message(STATUS "+ BoostStaticString populated: ${BoostStaticString_POPULATED}")

###############################################################################
# Fetch Boost::functional                                                      #
###############################################################################
FetchContent_Declare(BoostFunctional
  GIT_REPOSITORY https://github.com/boostorg/functional.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostFunctional)
FetchContent_GetProperties(BoostFunctional POPULATED BoostFunctional_POPULATED)
message(STATUS "+ BoostFunctional populated: ${BoostFunctional_POPULATED}")

###############################################################################
# Fetch Boost::typeof                                                      #
###############################################################################
FetchContent_Declare(BoostTypeof
  GIT_REPOSITORY https://github.com/boostorg/typeof.git
  GIT_TAG        "boost-${BOOST_SUPERPROJECT_VERSION}"
  GIT_SHALLOW ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(BoostTypeof)
FetchContent_GetProperties(BoostTypeof POPULATED BoostTypeof_POPULATED)
message(STATUS "+ BoostTypeof populated: ${BoostTypeof_POPULATED}")


###############################################################################
# Fetch OpenSSL::SSL OpenSSL::Crypto                                          #
###############################################################################
FetchContent_Declare(OpenSSL
  GIT_REPOSITORY https://github.com/janbar/openssl-cmake.git
  GIT_TAG        1.1.1w-20250419
  GIT_SHALLOW    ON
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(OpenSSL)
FetchContent_GetProperties(OpenSSL POPULATED OpenSSL_POPULATED)
message(STATUS "+ OpenSSL populated: ${OpenSSL_POPULATED}")

#add_subdirectory(${openssl_SOURCE_DIR} ${openssl_BINARY_DIR})

# set(OPENSSL_ROOT_DIR ${OpenSSL_BINARY_DIR})
# set(OPENSSL_INCLUDE_DIR "${OpenSSL_BINARY_DIR}/include")
# set(OPENSSL_INCLUDE_DIR "${OpenSSL_BINARY_DIR}/lib")
# set(OPENSSL_USE_STATIC_LIBS TRUE)  # nếu build static
# message(STATUS "+ OpenSSL_BINARY_DIR: ${OpenSSL_BINARY_DIR}")

###############################################################################
# Configure Build Process                                           #
###############################################################################
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

if (MSVC)
  add_compile_options(
    /std:c++20                # C++20
    /FS                       # Allow parallel PDB access
    /MP                       # Multi-process build
    /Zi                       # Debug info, == -g
    /W4                       # Warning level 4, == -Wall -Wextra. 
                              # Note that: the max warning level is /Wall. It includes unnecessary warnings from system headers, STL, Windows SDK...
    /GS                       # Stack protector, == -fstack-protector-strong
    /EHsc                     # Exception model (C++)
    /permissive-              # Strict standard conformance, == -Wpedantic
    /external:W0              # Suppress warnings from external headers
    /external:anglebrackets   # Treat headers included with <> as external
    /Zc:__cplusplus           # Standardize the __cplusplus macro value
    /Zc:inline                # Optimize inline function handling
    /Zc:preprocessor          # Use modern conforming preprocessor (MSVC 2019+)
    /d2FH4-                   # Workaround: Fix IntelliSense issues with /std:c++20 and STL (MSVC-specific)
    /sdl            # Format string + buffer checks. ~~-Wformat=2
    # /WX                     # Warnings as errors
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(
    -std=c++20                  # Enable C++20 standard
    -Wall                       # Enable most common warnings
    -Wextra                     # Enable extra (more aggressive) warnings
    #-Wnoexcept                  # Enables warnings if a function marked noexcept might actually throw
    #-O2                         # Enables deeper analysis of noexcept
    -Wpedantic                  # Enforce strict ISO C++ compliance
    -Wconversion                # Warn on implicit type conversions (e.g., int to float)
    -Wsign-conversion           # Warn on implicit sign conversions (e.g., unsigned to signed)
    -Wshadow                    # Warn when a variable declaration shadows one from outer scope
    -Wnon-virtual-dtor          # Warn if a class with virtual functions has a non-virtual destructor
    -Wold-style-cast            # Warn on C-style casts (use static_cast, etc. instead)
    -Wnull-dereference          # Warn on potential null pointer dereference
    -Wdouble-promotion          # Warn when float is implicitly promoted to double
    -Wformat=2                  # Enable strict format string checking (e.g., printf/scanf)
    -Wimplicit-fallthrough      # Warn when a switch case falls through without annotation
    -fdiagnostics-color=always  # Force colored diagnostics output (useful in IDE/terminals)
    -fstack-protector-strong    # Add stack overflow protection (stronger than -fstack-protector)
    -fno-common                 # Prevents multiple global variable definitions with same name
    -g                          # Generate debug information
  )

endif()


###############################################################################
# Detect and print source files                                               #
# TODO: Use a more robust method to detect source files                       #
###############################################################################
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  src/*.cpp
  src/*.h
)

message(STATUS "---------------------")
message(STATUS "List of source files:")
message(STATUS "---------------------")
foreach(src_file ${SRC_FILES})
  message(STATUS "+ ${src_file}")
endforeach()


###############################################################################
# Activate testing if BUILD_TESTING is ON                                     #
###############################################################################
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
  include(CTest)
  add_subdirectory(tests)
endif()


###############################################################################
# Add executable and link libraries                                           #
###############################################################################

add_executable(bggclient 
  main.cpp
  ${SRC_FILES}
)

target_compile_features(bggclient PRIVATE cxx_std_20)
target_link_libraries(bggclient PRIVATE
  sfml-graphics 
  sfml-audio 
  sfml-window
  sfml-system
  sfml-main
  ImGui-SFML
  boost_assert
  boost_uuid
  boost_beast
  boost_json
  ssl 
  crypto
  spdlog
)

target_include_directories(bggclient PRIVATE 
  ${CMAKE_SOURCE_DIR}/src
)

###############################################################################
# Print all targets
###############################################################################
function(print_all_targets_recursive dir)
  # Get and print all targets from the current directory
  get_property(local_targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
  foreach(tgt IN LISTS local_targets)
    message(STATUS "+ ${tgt}")
  endforeach()

  # Find subdirectories then get and print all targets from them
  get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
  foreach(subdir IN LISTS subdirs)
    print_all_targets_recursive("${subdir}")
  endforeach()
endfunction()

# Call the recursive funtion
message(STATUS "----------------")
message(STATUS "List of targets:")
message(STATUS "----------------")
print_all_targets_recursive(${CMAKE_SOURCE_DIR})

get_target_property(INCLUDE_DIRS bggclient INCLUDE_DIRECTORIES)
message(STATUS "Include directories for bggclient: ${INCLUDE_DIRS}")


###############################################################################
# Add necessary macros                                                        #
###############################################################################
target_compile_definitions(bggclient PRIVATE
  $<$<CONFIG:Debug>:_DEBUG> #Macro _DEBUG
  $<$<CONFIG:Release>:NDEBUG> #Macro NDEBUG for disbleing assert() 
  $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0601> #Macro _WIN32_WINNT
)


###############################################################################
# Install App                                                                 #
###############################################################################
install(TARGETS bggclient
  RUNTIME DESTINATION bin
)
install(DIRECTORY assets/
  DESTINATION share/${PROJECT_NAME}/assets
)
install(DIRECTORY fonts/
  DESTINATION share/${PROJECT_NAME}/fonts
)