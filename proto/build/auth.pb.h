// Generated by the protocol buffer compiler.  DO NOT EDIT!
// NO CHECKED-IN PROTOBUF GENCODE
// source: auth.proto
// Protobuf C++ Version: 5.27.2

#ifndef GOOGLE_PROTOBUF_INCLUDED_auth_2eproto_2epb_2eh
#define GOOGLE_PROTOBUF_INCLUDED_auth_2eproto_2epb_2eh

#include <limits>
#include <string>
#include <type_traits>
#include <utility>

#include "google/protobuf/runtime_version.h"
#if PROTOBUF_VERSION != 5027002
#error "Protobuf C++ gencode is built with an incompatible version of"
#error "Protobuf C++ headers/runtime. See"
#error "https://protobuf.dev/support/cross-version-runtime-guarantee/#cpp"
#endif
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/arena.h"
#include "google/protobuf/arenastring.h"
#include "google/protobuf/generated_message_tctable_decl.h"
#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/metadata_lite.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/message.h"
#include "google/protobuf/repeated_field.h"  // IWYU pragma: export
#include "google/protobuf/extension_set.h"  // IWYU pragma: export
#include "google/protobuf/unknown_field_set.h"
#include "sign_up.pb.h"
#include "sign_in.pb.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"

#define PROTOBUF_INTERNAL_EXPORT_auth_2eproto

namespace google {
namespace protobuf {
namespace internal {
class AnyMetadata;
}  // namespace internal
}  // namespace protobuf
}  // namespace google

// Internal implementation detail -- do not use these members.
struct TableStruct_auth_2eproto {
  static const ::uint32_t offsets[];
};
extern const ::google::protobuf::internal::DescriptorTable
    descriptor_table_auth_2eproto;
class Auth;
struct AuthDefaultTypeInternal;
extern AuthDefaultTypeInternal _Auth_default_instance_;
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google


// ===================================================================


// -------------------------------------------------------------------

class Auth final : public ::google::protobuf::Message
/* @@protoc_insertion_point(class_definition:Auth) */ {
 public:
  inline Auth() : Auth(nullptr) {}
  ~Auth() override;
  template <typename = void>
  explicit PROTOBUF_CONSTEXPR Auth(
      ::google::protobuf::internal::ConstantInitialized);

  inline Auth(const Auth& from) : Auth(nullptr, from) {}
  inline Auth(Auth&& from) noexcept
      : Auth(nullptr, std::move(from)) {}
  inline Auth& operator=(const Auth& from) {
    CopyFrom(from);
    return *this;
  }
  inline Auth& operator=(Auth&& from) noexcept {
    if (this == &from) return *this;
    if (GetArena() == from.GetArena()
#ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetArena() != nullptr
#endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const
      ABSL_ATTRIBUTE_LIFETIME_BOUND {
    return _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance);
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields()
      ABSL_ATTRIBUTE_LIFETIME_BOUND {
    return _internal_metadata_.mutable_unknown_fields<::google::protobuf::UnknownFieldSet>();
  }

  static const ::google::protobuf::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::google::protobuf::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::google::protobuf::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Auth& default_instance() {
    return *internal_default_instance();
  }
  enum PayloadCase {
    kSignUp = 2,
    kSignIn = 3,
    PAYLOAD_NOT_SET = 0,
  };
  static inline const Auth* internal_default_instance() {
    return reinterpret_cast<const Auth*>(
        &_Auth_default_instance_);
  }
  static constexpr int kIndexInFileMessages = 0;
  friend void swap(Auth& a, Auth& b) { a.Swap(&b); }
  inline void Swap(Auth* other) {
    if (other == this) return;
#ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetArena() != nullptr && GetArena() == other->GetArena()) {
#else   // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetArena() == other->GetArena()) {
#endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::google::protobuf::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Auth* other) {
    if (other == this) return;
    ABSL_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Auth* New(::google::protobuf::Arena* arena = nullptr) const final {
    return ::google::protobuf::Message::DefaultConstruct<Auth>(arena);
  }
  using ::google::protobuf::Message::CopyFrom;
  void CopyFrom(const Auth& from);
  using ::google::protobuf::Message::MergeFrom;
  void MergeFrom(const Auth& from) { Auth::MergeImpl(*this, from); }

  private:
  static void MergeImpl(
      ::google::protobuf::MessageLite& to_msg,
      const ::google::protobuf::MessageLite& from_msg);

  public:
  bool IsInitialized() const {
    return true;
  }
  ABSL_ATTRIBUTE_REINITIALIZES void Clear() final;
  ::size_t ByteSizeLong() const final;
  ::uint8_t* _InternalSerialize(
      ::uint8_t* target,
      ::google::protobuf::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::google::protobuf::Arena* arena);
  void SharedDtor();
  void InternalSwap(Auth* other);
 private:
  friend class ::google::protobuf::internal::AnyMetadata;
  static ::absl::string_view FullMessageName() { return "Auth"; }

 protected:
  explicit Auth(::google::protobuf::Arena* arena);
  Auth(::google::protobuf::Arena* arena, const Auth& from);
  Auth(::google::protobuf::Arena* arena, Auth&& from) noexcept
      : Auth(arena) {
    *this = ::std::move(from);
  }
  const ::google::protobuf::Message::ClassData* GetClassData() const final;

 public:
  ::google::protobuf::Metadata GetMetadata() const;
  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------
  enum : int {
    kIdFieldNumber = 1,
    kSignUpFieldNumber = 2,
    kSignInFieldNumber = 3,
  };
  // int32 id = 1;
  void clear_id() ;
  ::int32_t id() const;
  void set_id(::int32_t value);

  private:
  ::int32_t _internal_id() const;
  void _internal_set_id(::int32_t value);

  public:
  // .Sign_up sign_up = 2;
  bool has_sign_up() const;
  private:
  bool _internal_has_sign_up() const;

  public:
  void clear_sign_up() ;
  const ::Sign_up& sign_up() const;
  PROTOBUF_NODISCARD ::Sign_up* release_sign_up();
  ::Sign_up* mutable_sign_up();
  void set_allocated_sign_up(::Sign_up* value);
  void unsafe_arena_set_allocated_sign_up(::Sign_up* value);
  ::Sign_up* unsafe_arena_release_sign_up();

  private:
  const ::Sign_up& _internal_sign_up() const;
  ::Sign_up* _internal_mutable_sign_up();

  public:
  // .Sign_in sign_in = 3;
  bool has_sign_in() const;
  private:
  bool _internal_has_sign_in() const;

  public:
  void clear_sign_in() ;
  const ::Sign_in& sign_in() const;
  PROTOBUF_NODISCARD ::Sign_in* release_sign_in();
  ::Sign_in* mutable_sign_in();
  void set_allocated_sign_in(::Sign_in* value);
  void unsafe_arena_set_allocated_sign_in(::Sign_in* value);
  ::Sign_in* unsafe_arena_release_sign_in();

  private:
  const ::Sign_in& _internal_sign_in() const;
  ::Sign_in* _internal_mutable_sign_in();

  public:
  void clear_payload();
  PayloadCase payload_case() const;
  // @@protoc_insertion_point(class_scope:Auth)
 private:
  class _Internal;
  void set_has_sign_up();
  void set_has_sign_in();
  inline bool has_payload() const;
  inline void clear_has_payload();
  friend class ::google::protobuf::internal::TcParser;
  static const ::google::protobuf::internal::TcParseTable<
      0, 3, 2,
      0, 2>
      _table_;

  static constexpr const void* _raw_default_instance_ =
      &_Auth_default_instance_;

  friend class ::google::protobuf::MessageLite;
  friend class ::google::protobuf::Arena;
  template <typename T>
  friend class ::google::protobuf::Arena::InternalHelper;
  using InternalArenaConstructable_ = void;
  using DestructorSkippable_ = void;
  struct Impl_ {
    inline explicit constexpr Impl_(
        ::google::protobuf::internal::ConstantInitialized) noexcept;
    inline explicit Impl_(::google::protobuf::internal::InternalVisibility visibility,
                          ::google::protobuf::Arena* arena);
    inline explicit Impl_(::google::protobuf::internal::InternalVisibility visibility,
                          ::google::protobuf::Arena* arena, const Impl_& from,
                          const Auth& from_msg);
    ::int32_t id_;
    union PayloadUnion {
      constexpr PayloadUnion() : _constinit_{} {}
      ::google::protobuf::internal::ConstantInitialized _constinit_;
      ::Sign_up* sign_up_;
      ::Sign_in* sign_in_;
    } payload_;
    mutable ::google::protobuf::internal::CachedSize _cached_size_;
    ::uint32_t _oneof_case_[1];
    PROTOBUF_TSAN_DECLARE_MEMBER
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_auth_2eproto;
};

// ===================================================================




// ===================================================================


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// -------------------------------------------------------------------

// Auth

// int32 id = 1;
inline void Auth::clear_id() {
  ::google::protobuf::internal::TSanWrite(&_impl_);
  _impl_.id_ = 0;
}
inline ::int32_t Auth::id() const {
  // @@protoc_insertion_point(field_get:Auth.id)
  return _internal_id();
}
inline void Auth::set_id(::int32_t value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:Auth.id)
}
inline ::int32_t Auth::_internal_id() const {
  ::google::protobuf::internal::TSanRead(&_impl_);
  return _impl_.id_;
}
inline void Auth::_internal_set_id(::int32_t value) {
  ::google::protobuf::internal::TSanWrite(&_impl_);
  _impl_.id_ = value;
}

// .Sign_up sign_up = 2;
inline bool Auth::has_sign_up() const {
  return payload_case() == kSignUp;
}
inline bool Auth::_internal_has_sign_up() const {
  return payload_case() == kSignUp;
}
inline void Auth::set_has_sign_up() {
  _impl_._oneof_case_[0] = kSignUp;
}
inline ::Sign_up* Auth::release_sign_up() {
  // @@protoc_insertion_point(field_release:Auth.sign_up)
  if (payload_case() == kSignUp) {
    clear_has_payload();
    auto* temp = _impl_.payload_.sign_up_;
    if (GetArena() != nullptr) {
      temp = ::google::protobuf::internal::DuplicateIfNonNull(temp);
    }
    _impl_.payload_.sign_up_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const ::Sign_up& Auth::_internal_sign_up() const {
  return payload_case() == kSignUp ? *_impl_.payload_.sign_up_ : reinterpret_cast<::Sign_up&>(::_Sign_up_default_instance_);
}
inline const ::Sign_up& Auth::sign_up() const ABSL_ATTRIBUTE_LIFETIME_BOUND {
  // @@protoc_insertion_point(field_get:Auth.sign_up)
  return _internal_sign_up();
}
inline ::Sign_up* Auth::unsafe_arena_release_sign_up() {
  // @@protoc_insertion_point(field_unsafe_arena_release:Auth.sign_up)
  if (payload_case() == kSignUp) {
    clear_has_payload();
    auto* temp = _impl_.payload_.sign_up_;
    _impl_.payload_.sign_up_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline void Auth::unsafe_arena_set_allocated_sign_up(::Sign_up* value) {
  // We rely on the oneof clear method to free the earlier contents
  // of this oneof. We can directly use the pointer we're given to
  // set the new value.
  clear_payload();
  if (value) {
    set_has_sign_up();
    _impl_.payload_.sign_up_ = value;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:Auth.sign_up)
}
inline ::Sign_up* Auth::_internal_mutable_sign_up() {
  if (payload_case() != kSignUp) {
    clear_payload();
    set_has_sign_up();
    _impl_.payload_.sign_up_ =
        ::google::protobuf::Message::DefaultConstruct<::Sign_up>(GetArena());
  }
  return _impl_.payload_.sign_up_;
}
inline ::Sign_up* Auth::mutable_sign_up() ABSL_ATTRIBUTE_LIFETIME_BOUND {
  ::Sign_up* _msg = _internal_mutable_sign_up();
  // @@protoc_insertion_point(field_mutable:Auth.sign_up)
  return _msg;
}

// .Sign_in sign_in = 3;
inline bool Auth::has_sign_in() const {
  return payload_case() == kSignIn;
}
inline bool Auth::_internal_has_sign_in() const {
  return payload_case() == kSignIn;
}
inline void Auth::set_has_sign_in() {
  _impl_._oneof_case_[0] = kSignIn;
}
inline ::Sign_in* Auth::release_sign_in() {
  // @@protoc_insertion_point(field_release:Auth.sign_in)
  if (payload_case() == kSignIn) {
    clear_has_payload();
    auto* temp = _impl_.payload_.sign_in_;
    if (GetArena() != nullptr) {
      temp = ::google::protobuf::internal::DuplicateIfNonNull(temp);
    }
    _impl_.payload_.sign_in_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const ::Sign_in& Auth::_internal_sign_in() const {
  return payload_case() == kSignIn ? *_impl_.payload_.sign_in_ : reinterpret_cast<::Sign_in&>(::_Sign_in_default_instance_);
}
inline const ::Sign_in& Auth::sign_in() const ABSL_ATTRIBUTE_LIFETIME_BOUND {
  // @@protoc_insertion_point(field_get:Auth.sign_in)
  return _internal_sign_in();
}
inline ::Sign_in* Auth::unsafe_arena_release_sign_in() {
  // @@protoc_insertion_point(field_unsafe_arena_release:Auth.sign_in)
  if (payload_case() == kSignIn) {
    clear_has_payload();
    auto* temp = _impl_.payload_.sign_in_;
    _impl_.payload_.sign_in_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline void Auth::unsafe_arena_set_allocated_sign_in(::Sign_in* value) {
  // We rely on the oneof clear method to free the earlier contents
  // of this oneof. We can directly use the pointer we're given to
  // set the new value.
  clear_payload();
  if (value) {
    set_has_sign_in();
    _impl_.payload_.sign_in_ = value;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:Auth.sign_in)
}
inline ::Sign_in* Auth::_internal_mutable_sign_in() {
  if (payload_case() != kSignIn) {
    clear_payload();
    set_has_sign_in();
    _impl_.payload_.sign_in_ =
        ::google::protobuf::Message::DefaultConstruct<::Sign_in>(GetArena());
  }
  return _impl_.payload_.sign_in_;
}
inline ::Sign_in* Auth::mutable_sign_in() ABSL_ATTRIBUTE_LIFETIME_BOUND {
  ::Sign_in* _msg = _internal_mutable_sign_in();
  // @@protoc_insertion_point(field_mutable:Auth.sign_in)
  return _msg;
}

inline bool Auth::has_payload() const {
  return payload_case() != PAYLOAD_NOT_SET;
}
inline void Auth::clear_has_payload() {
  _impl_._oneof_case_[0] = PAYLOAD_NOT_SET;
}
inline Auth::PayloadCase Auth::payload_case() const {
  return Auth::PayloadCase(_impl_._oneof_case_[0]);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include "google/protobuf/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_INCLUDED_auth_2eproto_2epb_2eh
